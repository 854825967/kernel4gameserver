/* 
 * File:   TimerHandler.h
 * Author: alax
 *
 * Created on March 5, 2015, 1:55 PM
 */

#ifndef __TimerHandler_h__
#define	__TimerHandler_h__

#include "Kernel.h"

enum TimerStatus {
    TIMER_STATUS_RUN = 0,
    TIMER_STATUS_PAUSE,
    TIMER_STATUS_WAITING_REMOVE
};

enum {
    LOOP_FOREVER = -1,
    LOOP_OVER = 0
};

class TimerHandler {
public:
    explicit TimerHandler():m_sStatus(TIMER_STATUS_WAITING_REMOVE), m_sInterval(0),
            m_sNextRunTick(0), m_sID(-1), m_sLoop(-1), m_pTimer(NULL) {}
    ~TimerHandler() {}
    
    inline bool IsRunTime(const s64 now) { // args now is mill second
        return m_sStatus == TIMER_STATUS_RUN
                && m_sLoop != LOOP_OVER 
                && now >= m_sNextRunTick;
    }
    
    inline void OnTimer(const s64 now) {
        TASSERT(m_pTimer
                && m_sStatus == TIMER_STATUS_RUN
                && m_sLoop != LOOP_OVER 
                && now >= m_sNextRunTick, 
                "this timer can't be run");
        
        m_pTimer->OnTime(Kernel::getInstance(), m_sID, m_sNextRunTick);
        if (m_sLoop > LOOP_OVER) {
            m_sLoop --;
        }
        
        if (m_sLoop != LOOP_OVER) {
            m_sNextRunTick += m_sInterval;
        } else {
            m_sStatus = TIMER_STATUS_WAITING_REMOVE;
            m_pTimer->OnTerminate(Kernel::getInstance(), m_sID, true, now);
        }
    }
        
    s8 m_sStatus;
    s64 m_sInterval;
    s64 m_sNextRunTick;
    s32 m_sID;
    s64 m_sLoop;
    tcore::ITimer * m_pTimer;
};

#endif	/* __TimerHandler_h__ */

