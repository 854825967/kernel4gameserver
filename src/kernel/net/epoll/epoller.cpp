#include "epoller.h"
#include "Tools.h"
#include "Kernel.h"
#include "Header.h"
#include "SPipe.h"
#include "CPipe.h"
#include "configmgr/Configmgr.h"
using namespace tcore;

epoller::epoller() : m_lEpollFD(-1) {

}

epoller::~epoller() {
    if (m_lEpollFD != -1) {
        close(m_lEpollFD);
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

    if (m_lEpollFD != -1) {
        ECHO("epoll is created");
        return false;
    }

    m_lEpollFD = epoll_create(EPOLL_DESC_COUNT);
    if (-1 == m_lEpollFD) {
        ECHO("create epoll error %s", strerror(errno));
        return false;
    }

    m_pWorkerAry = NEW epollWorker[m_nWorkerCount];
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

epollWorker * epoller::BalancingWorker()  {
    epollWorker * pWorker = NULL;
    if (m_pWorkerAry) {
        pWorker = m_pWorkerAry;
        for (s32 i=1; i<m_nWorkerCount; i++) {
            if (m_pWorkerAry[i].GetOPTCount() < pWorker->GetOPTCount()) {
                pWorker = &(m_pWorkerAry[i]);
            }
        }
    }
    
    return pWorker;
}

bool epoller::AddServer(ITcpServer * server, const char * ip, const s32 port) {
    TASSERT(server != NULL, "wtf where is server");
    s64 lSocket = -1;
    struct timeval tv;
    
    SPipe * pSPipe = NEW SPipe;
    struct sockaddr_in addr;
    tools::SafeMemset(&addr, sizeof(addr), 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(inet_addr(ip));
    addr.sin_port = htons(port);
    
    if (-1 == (lSocket = socket(AF_INET, SOCK_STREAM, 0))
        || 0 != setsockopt(lSocket, SOL_SOCKET, SO_REUSEADDR, (const char*) &tv, sizeof (tv))
        || !setnonblocking(lSocket)
        || 0 != bind(lSocket, (sockaddr *) & addr, sizeof (addr))
        || 0 != listen(lSocket, 200)) {
        TASSERT(false, "socket error %s", strerror(errno));
        close(lSocket);
        return false;
    }
    
    pSPipe->Relate(server, lSocket, m_lEpollFD);
    epoll_event ev;
    ev.data.ptr = (void *)pSPipe->GetEvent();
    ev.events = EPOLLIN | EPOLLET;

    s32 res = epoll_ctl(m_lEpollFD, EPOLL_CTL_ADD, lSocket, &ev);
    TASSERT(res == 0, strerror(errno));

    return true;
}

bool epoller::AddClient(ITcpSession * client, const char * ip, const s32 port) {
    s64 lSocket = -1;
    struct timeval tv;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    if (-1 == (lSocket = socket(AF_INET, SOCK_STREAM, 0))
        || 0 != setsockopt(lSocket, SOL_SOCKET, SO_REUSEADDR, (const char*) &tv, sizeof (tv))
        || !setnonblocking(lSocket)
        || inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        ECHO("socket error %s", strerror(errno));
        close(lSocket);
        return false;
    }

    s32 ret = connect(lSocket, (struct sockaddr *) &addr, sizeof (addr));
    if (ret == 0) {
        CPipe * pCPipe = CPipe::Create();
        s64 lEpollFD = BalancingWorker()->GetEpollFD();
        pCPipe->Relate(client, lSocket, lEpollFD);
        pCPipe->DoConnect();
        client->OnConnected(Kernel::getInstance());
    } else if (ret < 0 && errno != EINPROGRESS) {
        ECHO("connect error %s", strerror(errno));
        client->OnConnectFailed(Kernel::getInstance());
        return false;
    } else {
        CPipe * pCPipe = CPipe::Create();
        s64 lEpollFD = BalancingWorker()->GetEpollFD();
        pCPipe->Relate(client, lSocket, lEpollFD);
        epoll_event ev;
        ev.data.ptr = (void *)pCPipe->GetEvent();
        ev.events = EPOLLOUT | EPOLLET;

        s32 res = epoll_ctl(m_lEpollFD, EPOLL_CTL_ADD, lSocket, &ev);
        TASSERT(res == 0, strerror(errno));
    }

    return true;
}

s64 epoller::DealEvent(s64 overtime) {
    s64 lTick = tools::GetTimeMillisecond();
    epoll_event events[EPOLLER_EVENTS_COUNT];
    memset(&events, 0, sizeof(events));  
    errno = 0;
    
    int retCount = epoll_wait(m_lEpollFD, events, EPOLLER_EVENTS_COUNT, 5);
    if (retCount == -1) {
        TASSERT(errno == EINTR, "epoll_wait err! %s", strerror(errno));
        return tools::GetTimeMillisecond() - lTick;
    }

    for (s32 i=0; i<retCount; i++) {
        epollerEvent * pEvent = (epollerEvent *)events[i].data.ptr;
        switch (pEvent->type) {
            case SO_ACCEPT:
            {
                SPipe * pSPipe = (SPipe *)pEvent->pData;
                if (events[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                    pSPipe->GetHost()->Error(Kernel::getInstance(), NULL);
                } else if (events[i].events & EPOLLIN) {
                    pSPipe->DoAccept();
                }
                
                break;
            }
            case SO_CONNECT:
            {
                CPipe * pCPipe = (CPipe *)pEvent->pData;
                if (events[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                    pCPipe->GetHost()->OnConnectFailed(Kernel::getInstance());
                    pCPipe->GetHost()->m_pPipe = NULL;
                    pCPipe->Release();
                } else if (events[i].events & EPOLLOUT) {
                    pCPipe->DoConnect();
                }
                break;
            }
            default:
                TASSERT(false, "wtf");
                break;
        }
    }
    
    return tools::GetTimeMillisecond() - lTick;
}

s64 epoller::Processing(s64 overtime) {
    s64 lTick = tools::GetTimeMillisecond();
    s64 lUser = tools::GetTimeMillisecond();
    
    DealEvent(overtime/(m_nWorkerCount + 1));
    
    for (s32 i=0; i<m_nWorkerCount; i++) {
        m_pWorkerAry[i].Processing(overtime/(m_nWorkerCount + 1));
    }
    
    return 0;
}

