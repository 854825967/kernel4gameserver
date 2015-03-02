/* 
 * File:   epoller_wait.h
 * Author: alax
 *
 * Created on January 15, 2015, 12:09 AM
 */

#ifndef __epoll_waiter_h__
#define	__epoll_waiter_h__

#include <sys/epoll.h>
#include "CThread.h"
#include "TQueue.h"
#include "TPool.h"
#include "IKernel.h"
#include "header.h"
using namespace tlib;

class epoller;

class epoll_waiter : public CThread {
public:

    epoll_waiter() {
        m_status = THREAD_STOPED;
        m_pEpoller = NULL;
    }

    ~epoll_waiter() {

    }

    inline void SetEpoller(epoller * p) {
        m_pEpoller = p;
        TASSERT(m_pEpoller != NULL, "where is epoller");
    }

    virtual void Run();

    inline void SafeStop() {
        if (THREAD_WORKING == m_status) {
            m_status = THREAD_STOPPING;

            while (m_status != THREAD_STOPED) {
                CSLEEP(1);
            }
        }
    }

private:
    s8 m_status;
    epoller * m_pEpoller;
};

#endif	/* __epoll_waiter_h__ */

