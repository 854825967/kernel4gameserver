/* 
 * File:   epollworker.h
 * Author: alax
 *
 * Created on January 15, 2015, 2:04 AM
 */

#ifndef __epollworker_h__
#define	__epollworker_h__

#include "epollheader.h"
using namespace tlib;

#define EPOLLER_DATA_QUEUE_SIZE 4096


class epollWorker : public tlib::CThread {
    typedef CHashMap<s64, tcore::ISocket *> SOCKET_CLIENT_RELATION;
public:
    epollWorker() : m_nStatus(THREAD_STOPED), m_lOptCount(0), m_lEpollFd(-1) {}
    bool Initialize();
    void Run();
    s64 Processing(s64 overtime);
    
    inline bool Terminate() {
        if (THREAD_WORKING == m_nStatus) {
            m_nStatus = THREAD_STOPPING;
            
            while (m_nStatus != THREAD_STOPED) {
                CSLEEP(1);
            }
            
            return true;
        }
        
        return false;
    }
    
    inline s64 GetEpollFD() {
        TASSERT(m_lEpollFd != -1, "wtf");
        return m_lEpollFd;
    }
    
    inline s64 GetOPTCount() {
        return m_lOptCount;
    }
    
private:
    s8 m_nStatus;
    s64 m_lEpollFd;
    s64 m_lOptCount;
    IOEVENT_POOL m_oEventPool;
    IOEVENT_QUEUE m_oEventQueue;
};

#endif	/* __epoll_worker_h__ */
