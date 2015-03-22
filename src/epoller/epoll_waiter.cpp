#include "epoll_waiter.h"
#include "epoller.h"

void epoll_waiter::Run() { //public cthread
    epoll_event events[EPOLLER_EVENTS_COUNT];
    m_status = THREAD_WORKING;
    while (true) {
        memset(&events, 0, sizeof(events));    
        errno = 0;
        int retCount = epoll_wait(m_pEpoller->GetEpollDesc(), events, EPOLLER_EVENTS_COUNT, 15);
        if (retCount == -1 && errno != EINTR) {
            ECHO("epoll_wait err! %s", strerror(errno));
            TASSERT(false, strerror(errno));
            m_status = THREAD_STOPED;
            return;
        }

        if (0 == retCount || retCount == -1) {
            if (m_status == THREAD_STOPPING) {
                m_status = THREAD_STOPED;
                return;
            }
            
            continue;
        }
        
        for (s32 i=0; i<retCount; i++) {
            epoller_data * p = (epoller_data *)events[i].data.ptr;
            p->flags = events[i].events;
            switch (p->opt) {
                case tcore::SO_ACCEPT:
                {
                    m_pEpoller->AddIO(p);
                    break;
                }
                case tcore::SO_CONNECT:
                {
                    m_pEpoller->AddIO(p);
                    break;
                }
                case tcore::SO_TCPRECV:
                {
                    m_pEpoller->AddEvent(p);
                    break;
                }
//                case tcore::SO_UDPIO:
//                {
//                    m_pEpoller->AddEvent(p);
//                    break;
//                }
            }
        }
    }
}
