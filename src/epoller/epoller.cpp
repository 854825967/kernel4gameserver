#include "Tools.h"
#include "Kernel.h"
#include "epoller.h"
#include "configmgr/Configmgr.h"
#include "epoller_worker.h"
using namespace tcore;

tlib::TPool<struct epoller_event, true, 128> g_EpollerDataPool;

epoller::epoller() : m_epollfd(-1), m_nWorkerCount(0) {

}

epoller::~epoller() {
    if (m_epollfd != -1) {
        shut_socket(m_epollfd);
    }
}

INetengine * epoller::getInstance() {
    static INetengine * p = NULL;
    if (!p) {
        p = NEW epoller;
        if (!p->Redry()) {
            TASSERT(false, "epoller can't be ready");
            delete p;
            p = NULL;
        }
    }
    
    return p;
}

bool epoller::Redry() {

    return true;
}

bool epoller::Initialize() {
    m_nWorkerCount = Configmgr::getInstance()->GetCoreConfig()->sNetThdCount;

    if (m_epollfd != -1) {
        ECHO("epoll is created");
        return false;
    }

    m_epollfd = epoll_create(EPOLL_DESC_COUNT);
    if (-1 == m_epollfd) {
        ECHO("create epoll error %s", strerror(errno));
        m_epollfd = -1;
        return false;
    }

    m_pWorkerAry = NEW epoller_worker[m_nWorkerCount];
    TASSERT(m_pWorkerAry, "wtf");
    for (s32 i=0; i<m_nWorkerCount; i++) {
        bool res = m_pWorkerAry[i].Initialize();
        TASSERT(res, "wtf");
        m_pWorkerAry[i].Start();
    }
    
    return true;
}

bool epoller::Destory() {
    if (m_pWorkerAry) {
        for (s32 i=0; i<m_nWorkerCount; i++) {
            m_pWorkerAry[i].Terminate();
        }
        delete[] m_pWorkerAry;
    }
    
    delete this;
    return true;
}

epoller_worker * epoller::BalancingWorker()  {
    epoller_worker * pWorker = NULL;
    if (m_pWorkerAry) {
        pWorker = m_pWorkerAry;
        for (s32 i=1; i<m_nWorkerCount; i++) {
            if (m_pWorkerAry[i].GetSocketHandlerCount() < pWorker->GetSocketHandlerCount()) {
                pWorker = &(m_pWorkerAry[i]);
            }
        }
        
    }
    
    return pWorker;
}

bool epoller::AddServer(ITcpServer * server) {
    if ((server->socket_handler = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        TASSERT(false, "socket error %s", strerror(errno));
        return false;
    }

    struct timeval tv;
    if (0 != setsockopt(server->socket_handler, SOL_SOCKET, SO_REUSEADDR, (const char*) &tv, sizeof (tv))) {
        TASSERT(false, "setsockopt error %s", strerror(errno));
        return false;
    }

    if (!setnonblocking(server->socket_handler)) {
        TASSERT(false, "setnonbolocking error %s", strerror(errno));
        return false;
    }

    epoller_event * p = g_EpollerDataPool.Create();
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
        shut_socket(server->socket_handler);
        server->socket_handler = -1;
        return false;
    }

    if (listen(server->socket_handler, 4096) != 0) {
        ECHO("add server error : %s", strerror(errno));
        shut_socket(server->socket_handler);
        server->socket_handler = -1;
        return false;
    }

    s32 res = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, server->socket_handler, &ev);
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
        //client->Error(Kernel::getInstance(), SO_CONNECT, "");
    } else if (ret < 0 && errno != EINPROGRESS) {
        ECHO("connect error %s", strerror(errno));
        return false;
    } else {
        epoller_event * p = g_EpollerDataPool.Create();
        p->opt = SO_CONNECT;
        p->user_ptr = client;

        epoll_event ev;
        ev.data.ptr = p;
        ev.events = EPOLLOUT | EPOLLET;

        s32 res = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, client->socket_handler, &ev);
        TASSERT(res == 0, strerror(errno));
    }

    return true;
}

s64 epoller::DealEvent(s64 overtime) {
    s64 lTick = tools::GetTimeMillisecond();
    epoll_event events[EPOLLER_EVENTS_COUNT];

    memset(&events, 0, sizeof(events));    
    errno = 0;
    int retCount = epoll_wait(m_epollfd, events, EPOLLER_EVENTS_COUNT, 5);
    if (retCount == -1) {
        TASSERT(errno == EINTR, "epoll_wait err! %s", strerror(errno));
        return tools::GetTimeMillisecond() - lTick;
    }

    for (s32 i=0; i<retCount; i++) {
        epoller_event * p = (epoller_event *)events[i].data.ptr;
        p->flags = events[i].events;
        switch (p->opt) {
            case tcore::SO_ACCEPT:
            {
                ITcpServer * pServer = (ITcpServer *)p->user_ptr;
                pServer->DoAccept(p, this);
                break;
            }
            case tcore::SO_CONNECT:
            {
                ITcpSocket * pClient = (ITcpSocket *)p->user_ptr;
                epoll_ctl(m_epollfd, EPOLL_CTL_DEL, pClient->socket_handler, NULL);
                pClient->DoConnect(p, this);
                g_EpollerDataPool.Recover(p);
                break;
            }
            default:
                TASSERT(false, "wtf");
                break;
        }
    }
    
    return tools::GetTimeMillisecond() - lTick;
}

s64 epoller::DonetIO(s64 overtime) {
    s64 lTick = tools::GetTimeMillisecond();
    s64 lUser = tools::GetTimeMillisecond();

    DealEvent(overtime/(m_nWorkerCount + 1));
    
    for (s32 i=0; i<m_nWorkerCount; i++) {
        m_pWorkerAry[i].DealEvent(overtime/(m_nWorkerCount + 1));
    }
    
    return 0;
}

