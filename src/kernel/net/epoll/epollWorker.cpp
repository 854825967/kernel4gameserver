#include "epollWorker.h"
#include "Kernel.h"
#include "configmgr/Configmgr.h"
#include "Header.h"
#include "CPipe.h"
using namespace tcore;

s64 epollWorker::Processing(s64 overtime) {
    s64 lTick = tools::GetTimeMillisecond();
    s64 lUse = 0;
    ioevent event;
    while (m_oEventQueue.Read(event)) {
         switch (event.type) {
            case IO_EVENT_TYPE_BREAK:
            case IO_EVENT_TYPE_RECV:
            {
                CPipe * pCPipe = event.pCPipe;
                TASSERT(pCPipe, "wtf");
                
                s32 nUse = 0;
                do {
                    pCPipe->m_oRecvStream.LockRead();
                    nUse = 0;
                    s32 nLeft = pCPipe->m_oRecvStream.size();
                    TASSERT(nLeft >= 0, "wtf %d", nLeft);
                    if (nLeft > 0) {
                        nUse = pCPipe->GetHost()->OnRecv(Kernel::getInstance(), 
                                    (void *)pCPipe->m_oRecvStream.buff(), nLeft);

                        if (nUse > 0) {
                            pCPipe->m_oRecvStream.out(nUse);
                        }
                    }
                    
                    pCPipe->m_oRecvStream.FreeRead();
                } while (0 != nUse);

                if (IO_EVENT_TYPE_BREAK == event.type) {
                    pCPipe->GetHost()->OnDisconnect(Kernel::getInstance());
                }
                
                break;
            }
            default:
                TASSERT(false, "wtf");
                break;
        }
         
        lUse = tools::GetTimeMillisecond();
        if (lUse - lTick >= overtime) {
            return lUse - lTick;
        }
    }
    
    return tools::GetTimeMillisecond() - lTick; 
}

bool epollWorker::Initialize() {
    m_lEpollFd = epoll_create(EPOLL_DESC_COUNT);
    if (-1 == m_lEpollFd) {
        ECHO("create epoll error %s", strerror(errno));
        m_lEpollFd = -1;
        return false;
    }

    return true;
}

void epollWorker::Run() {
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
        if (0 == (nRetCount = epoll_wait(m_lEpollFd, events, EPOLLER_EVENTS_COUNT, 5))) {
            CSLEEP(1);
            continue;
        } else if (nRetCount == -1) {
            TASSERT(errno == EINTR, "epoll_wait err! %s", strerror(errno));
            continue;
        }

        for (s32 i = 0; i < nRetCount; i++) {
            epollerEvent * p = (epollerEvent *) events[i].data.ptr;
            TASSERT(p->type == SO_CONNECT, "wtf");
            CPipe * pCPipe = (CPipe *)p->pData;
            
            s32 eventType = IO_EVENT_TYPE_COUNT;
            if (events[i].events & EPOLLIN && SS_ESTABLISHED == pCPipe->GetStatus()) {
                eventType = pCPipe->DoRecv();
            }
            
            if (events[i].events & EPOLLOUT && pCPipe->GetStatus() != SS_UNINITIALIZE) {
                s8 type = pCPipe->DoSend();
                if (eventType != IO_EVENT_TYPE_BREAK && type != IO_EVENT_TYPE_COUNT) {
                    eventType = type;
                }
            }
            
            if (events[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP) && pCPipe->GetStatus() != SS_UNINITIALIZE) {
                pCPipe->DoClose();
            }
            
            if (eventType != IO_EVENT_TYPE_COUNT) {
                ioevent event;
                event.type = eventType;
                event.pCPipe = pCPipe;
                m_oEventQueue.Add(event);
            }
        }
    }
}