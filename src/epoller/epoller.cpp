#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Tools.h"
#include "epoller.h"
#include "epoll_waiter.h"
#include "epoll_worker.h"
#include "configmgr/Configmgr.h"
using namespace tcore;

tlib::TPool<struct epoller_data, true, 81920> g_EpollerDataPool;

epoller::epoller() {
    m_epoll = 0;
    m_nWorkerCount = 0;
}

epoller::~epoller() {
    if (m_epoll != 0) {
        shut_socket(m_epoll);
    }
}

bool epoller::Redry() {

    return true;
}

bool epoller::Initialize() {
    s32 count = Configmgr::getInstance()->GetCoreConfig()->sNetThdCount;

    if (m_epoll != 0) {
        ECHO("epoll is created");
        return false;
    }

    m_epoll = epoll_create(EPOLL_DESC_COUNT);
    if (-1 == m_epoll) {
        ECHO("create epoll error %s", strerror(errno));
        m_epoll = 0;
        return false;
    }

    ECHO("epoll fd %d", m_epoll);

    m_nWorkerCount = count;
    m_pWorkerAry = NEW epoll_worker[count];
    m_pWaiter = NEW epoll_waiter;

    m_pWaiter->SetEpoller(this);
    m_pWaiter->Start();

    for (s32 i = 0; i < count; i++) {
        m_pWorkerAry[i].SetEpoller(this);
        m_pWorkerAry[i].Start();
    }

    return true;
}

bool epoller::Destory() {
    return true;
}

void epoller::Run() {

}

bool epoller::AddServer(ITcpServer * server) {
    if ((server->socket_handler = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        ECHO("socket error %s", strerror(errno));
        return false;
    }

    struct timeval tv;
    if (0 != setsockopt(server->socket_handler, SOL_SOCKET, SO_REUSEADDR, (const char*) &tv, sizeof (tv))) {
        ECHO("setsockopt error %s", strerror(errno));
        return false;
    }

    if (!setnonblocking(server->socket_handler)) {
        ECHO("setnonbolocking error %s", strerror(errno));
        return false;
    }

    epoller_data * p = g_EpollerDataPool.Create();
    p->opt = SO_ACCEPT;
    p->user_ptr = server;

    epoll_event ev;
    ev.data.ptr = p;
    ev.events = EPOLLIN | EPOLLET;

    memset(&server->m_addr, 0, sizeof (server->m_addr));
    server->m_addr.sin_family = AF_INET;
    server->m_addr.sin_addr.s_addr = htonl(inet_addr(server->ip));
    server->m_addr.sin_port = htons(server->port);

    if (bind(server->socket_handler, (sockaddr *) & server->m_addr, sizeof (server->m_addr)) != 0) {
        ECHO("add server error : %s", strerror(errno));
        server->Error(SO_ACCEPT, errno);
        shut_socket(server->socket_handler);
        server->socket_handler = 0;
        return false;
    }

    if (listen(server->socket_handler, 4096) != 0) {
        ECHO("add server error : %s", strerror(errno));
        server->Error(SO_ACCEPT, errno);
        shut_socket(server->socket_handler);
        server->socket_handler = 0;
        return false;
    }

    s32 res = epoll_ctl(m_epoll, EPOLL_CTL_ADD, server->socket_handler, &ev);
    TASSERT(res == 0, strerror(errno));

    return true;
}

bool epoller::AddClient(ITcpSocket * client) {
    if ((client->socket_handler = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        ECHO("socket error %s", strerror(errno));
        return false;
    }

    struct timeval tv;
    if (0 != setsockopt(client->socket_handler, SOL_SOCKET, SO_REUSEADDR, (const char*) &tv, sizeof (tv))) {
        ECHO("setsockopt error %s", strerror(errno));
        shut_socket(client->socket_handler);
        return false;
    }

    if (!setnonblocking(client->socket_handler)) {
        ECHO("setnonbolocking error %s", strerror(errno));
        shut_socket(client->socket_handler);
        return false;
    }

    memset(&client->m_addr, 0, sizeof (client->m_addr));
    client->m_addr.sin_family = AF_INET;
    client->m_addr.sin_port = htons(client->port);

    if (inet_pton(AF_INET, client->ip, &client->m_addr.sin_addr) <= 0) {
        ECHO("inet_pton error %s", strerror(errno));
        shut_socket(client->socket_handler);
        return false;
    }

    s32 ret = connect(client->socket_handler, (struct sockaddr *) &client->m_addr, sizeof (client->m_addr));
    if (ret == 0) {
        client->Error(SO_CONNECT, 0);
    } else if (ret < 0 && errno != EINPROGRESS) {
        client->Error(SO_CONNECT, errno);
        ECHO("connect error %s", strerror(errno));
        return false;
    } else {
        epoller_data * p = g_EpollerDataPool.Create();
        p->opt = SO_CONNECT;
        p->user_ptr = client;

        epoll_event ev;
        ev.data.ptr = p;
        ev.events = EPOLLOUT | EPOLLET;

        s32 res = epoll_ctl(m_epoll, EPOLL_CTL_ADD, client->socket_handler, &ev);
        TASSERT(res == 0, strerror(errno));
    }

    return true;
}

void epoller::AddEvent(struct epoller_data * data) {
    m_pWorkerAry[data->index % m_nWorkerCount].AddWork(data);
}

void epoller::AddIO(struct epoller_data * data) {
    m_oIOQueue.Add(data);
}

s64 epoller::DonetIO(s64 overtime) {
    s64 lTick = tools::GetTimeMillisecond();

    epoller_data * p;
    while (true) {
        if (m_oIOQueue.Read(p)) {
            switch (p->opt) {
                case SO_ACCEPT:
                {
                    ITcpServer * server = (ITcpServer *) p->user_ptr;
                    server->DoAccept(p->flags, this);
                    break;
                }
                case SO_CONNECT:
                {
                    ITcpSocket * client = (ITcpSocket *) p->user_ptr;
                    client->DoConnect(p->flags, this);
                    g_EpollerDataPool.Recover(p);
                    break;
                }
                case SO_TCPIO:
                {
                    ITcpSocket * client = (ITcpSocket *) p->user_ptr;
                    client->Error(p->opt, p->code);
                    if (p->code != -1 && p->len != 0) {
                        s32 use_size;
                        client->m_recvStream.LockRead();
                        use_size = client->Recv((void *) client->m_recvStream.buff(), client->m_recvStream.size());
                        client->m_recvStream.FreeRead();
                        client->m_recvStream.out(use_size);
                    } else {
                        client->Disconnect();
                    }

                    g_EpollerDataPool.Recover(p);

                    break;
                }
            }

        } else if (m_oIOQueue.IsEmpty()) {
            return tools::GetTimeMillisecond() - lTick;
        }

        s64 lDoneTick = tools::GetTimeMillisecond() - lTick;
        if (lDoneTick >= overtime) {
            return overtime - lDoneTick;
        }
    }

    return 0;
}

