#include "epoller_worker.h"
#include "Kernel.h"
#include "configmgr/Configmgr.h"
using namespace tcore;

void epoller_worker::SendDisconnectEvent(struct epoller_event * & pEvent) {
    ISocket * pClient = (ISocket *) pEvent->user_ptr;
    {
        CLock lock(&m_lock);
        SOCKET_CLIENT_RELATION::iterator itor = m_mapSocketClient.find(pClient->socket_handler);
        if (itor != m_mapSocketClient.end()) {
            if (itor->second == pClient) {
                m_mapSocketClient.erase(itor);
                m_nSockCount--;
                TASSERT(m_nSockCount >= 0, "wtf");
                shut_socket(pClient->socket_handler);
                pEvent->code = 0;
                pEvent->len = 0;
                pEvent->opt = SO_TCPRECV;
                m_queue.Add(pEvent);
            }
        }
    }
}

bool epoller_worker::RelateSocketClient(const s64 socket, tcore::ISocket * pClient) {
    TASSERT(socket == pClient->socket_handler, "wtf");
    {
        CLock lock(&m_lock);
        SOCKET_CLIENT_RELATION::iterator itor = m_mapSocketClient.find(socket);
        if (itor != m_mapSocketClient.end()) {
            TASSERT(false, "check ur epoller_worker, it's crazy");
            return false;
        }
        m_mapSocketClient.insert(make_pair(socket, pClient));
        m_nSockCount++;
        TASSERT(m_nSockCount > 0, "wtf");
    }

    epoller_event * p = g_EpollerDataPool.Create();
    p->opt = SO_TCPRECV;
    p->user_ptr = pClient;

    epoll_event ev;
    ev.data.ptr = p;
    ev.events = EPOLLIN | EPOLLOUT;

    s32 res = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, pClient->socket_handler, &ev);
    if (0 != res) {
        TASSERT(false, "ctl error %s", strerror(errno));
        CLock lock(&m_lock);
        SOCKET_CLIENT_RELATION::iterator itor = m_mapSocketClient.find(pClient->socket_handler);
        if (itor != m_mapSocketClient.end()) {
            m_mapSocketClient.erase(itor);
            g_EpollerDataPool.Recover(p);
        } else {
            TASSERT(false, "wtf");
        }
        return false;
    }
    
    return true;
}

s64 epoller_worker::DealEvent(s64 overtime) {
    s64 lTick = tools::GetTimeMillisecond();
    s64 lUse = 0;
    epoller_event * pEvent = NULL;
    while (m_queue.Read(pEvent)) {
         switch (pEvent->opt) {
            case tcore::SO_TCPRECV:
            {
                ITcpSocket * pClient = (ITcpSocket *)pEvent->user_ptr;
                TASSERT(pClient, "wtf");
                
                if (pEvent->len > 0) {
                    s32 nUse = 0;
                    do {
                        pClient->m_recvStream.LockRead();
                        s32 nLeft = pClient->m_recvStream.size();
                        if (nLeft > 0) {
                            nUse = pClient->Recv(Kernel::getInstance(), 
                                    (void *)pClient->m_recvStream.buff(), nLeft);
                        } else {
                            nUse = 0;
                        }
                        pClient->m_recvStream.FreeRead();
                        if (nUse > 0) {
                            pClient->m_recvStream.out(nUse);
                        }
                    } while (0 != nUse);
                }
                
                if (pEvent->code != 0 || pEvent->len == 0) {
                    pClient->Disconnect(Kernel::getInstance());
                }
                break;
            }
            default:
                TASSERT(false, "wtf");
                break;
        }
        g_EpollerDataPool.Recover(pEvent);
        
        lUse = tools::GetTimeMillisecond();
        if (lUse - lTick >= overtime) {
            return lUse - lTick;
        }
    }
    
    return tools::GetTimeMillisecond() - lTick; 
}

bool epoller_worker::Initialize() {
    m_epollfd = epoll_create(EPOLL_DESC_COUNT);
    if (-1 == m_epollfd) {
        ECHO("create epoll error %s", strerror(errno));
        m_epollfd = -1;
        return false;
    }

    return true;
}

void epoller_worker::Run() {
    m_nStatus = THREAD_WORKING;
    epoll_event events[EPOLLER_EVENTS_COUNT];
    while (true) {
        if (m_nStatus == THREAD_STOPPING) {
            m_nStatus = THREAD_STOPED;
            return;
        }

        memset(&events, 0, sizeof (events));
        errno = 0;
        s32 nRetCount = 0;
        if (0 == (nRetCount = epoll_wait(m_epollfd, events, EPOLLER_EVENTS_COUNT, 5))) {
            CSLEEP(1);
            continue;
        } else if (nRetCount == -1) {
            TASSERT(errno == EINTR, "epoll_wait err! %s", strerror(errno));
            continue;
        }

        for (s32 i = 0; i < nRetCount; i++) {
            epoller_event * p = (epoller_event *) events[i].data.ptr;
            p->flags = events[i].events;
            switch (p->opt) {
                case tcore::SO_TCPRECV:
                {
                    ITcpSocket * ts = (ITcpSocket *) p->user_ptr;
                    ts->DoIO(p, this);
                    break;
                }
                default:
                    TASSERT(false, "wtf");
                    break;
            }
        }
    }
}
