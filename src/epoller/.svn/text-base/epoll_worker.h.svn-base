/* 
 * File:   epoll_worker.h
 * Author: alax
 *
 * Created on January 15, 2015, 2:04 AM
 */

#ifndef __epoll_worker_h__
#define	__epoll_worker_h__

#include "CThread.h"
#include "header.h"
#include "TQueue.h"

class epoller;

class epoll_worker : public tlib::CThread {
public:
    epoll_worker() {
        m_status = THREAD_STOPED;
    }
    
    void Run();
    
    inline void AddWork(struct epoller_data * work) {
        m_queue.Add(work);
    }    
    
    inline void SafeStop() {
        if (THREAD_WORKING == m_status) {
            m_status = THREAD_STOPPING;
            
            while (m_status != THREAD_STOPED) {
                CSLEEP(1);
            }
        }
    }
        
    inline void SetEpoller(epoller * p) {
        m_pEpoller = p;
        TASSERT(m_pEpoller != NULL, "where is epoller");
    }
    
private:
    s8 m_status;
    epoller * m_pEpoller;
    tlib::TQueue<struct epoller_data *, false, EPOLLER_DATA_COUNT> m_queue;
};

#endif	/* __epoll_worker_h__ */

