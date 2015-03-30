/* 
 * File:   epoller_worker.h
 * Author: alax
 *
 * Created on January 15, 2015, 2:04 AM
 */

#ifndef __epoller_worker_h__
#define	__epoller_worker_h__

#include "epoller_header.h"
using namespace tlib;

#define EPOLLER_DATA_QUEUE_SIZE 4096

class epoller_worker : public tlib::CThread {
    typedef CHashMap<s64, tcore::ISocket *> SOCKET_CLIENT_RELATION;
public:
    epoller_worker() : m_nStatus(THREAD_STOPED), m_nSockCount(0), m_epollfd(-1) {}
    bool Initialize();
    void Run();
    s64 DealEvent(s64 overtime);
    
    inline void AddEvent(struct epoller_event * pEvent) {
        m_queue.Add(pEvent);
    }
    
    bool RelateSocketClient(const s64 socket, tcore::ISocket * pClient);
    void SendDisconnectEvent(struct epoller_event * & pEvent);
    
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
    
    inline s32 GetSocketHandlerCount() {
        return m_nSockCount;
    }
    
private:
    s8 m_nStatus;
    s64 m_epollfd;
    s32 m_nSockCount;
    tlib::TQueue<struct epoller_event *, false, EPOLLER_DATA_QUEUE_SIZE> m_queue;
    tlib::CLockUnit m_lock;
    SOCKET_CLIENT_RELATION m_mapSocketClient;
};

#endif	/* __epoll_worker_h__ */

