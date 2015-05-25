/* 
 * File:   Timermgr.h
 * Author: alax
 *
 * Created on March 5, 2015, 11:02 AM
 */

#ifndef __Timermgr_h__
#define	__Timermgr_h__

#include "ITimermgr.h"
#include "TPool.h"
#include <list>
#include <map>

class timer_index {
public:
    
    explicit timer_index(s32 id, const tcore::ITimer * p) {
        m_id = id;
        m_p = p;
    }
    
    timer_index & operator=(const timer_index & index) {
        tools::SafeMemcpy(this, sizeof(*this), &index, sizeof(*this));
        return *this;
    }

    bool operator==(const timer_index & index) const {
        return m_id == index.m_id && m_p == index.m_p;
    }

    bool operator<(const timer_index & index) const {
        return (this->m_id + (s64) this->m_p) < (index.m_id + (s64) index.m_p);
    }

private:
    s32 m_id;
    const tcore::ITimer * m_p;
};
class TimerHandler;

class Timermgr : public ITimermgr {
    typedef std::list<TimerHandler *> TIMER_LIST;
    typedef std::map<timer_index, TimerHandler *> TIMER_MAP;
    typedef std::map<s64, TIMER_LIST> TIMER_WHOOL;
public:
    static ITimermgr * getInstance();
    virtual bool Redry();
    virtual bool Initialize();
    virtual bool Destory();

    // tiemr interface 
    virtual bool StartTimer(s32 id, tcore::ITimer * timer, s64 interval, s64 delay, s64 loop);
    virtual bool KillTimer(s32 id, tcore::ITimer * timer);
    virtual bool KillTimer(tcore::ITimer * timer);

    virtual s64 Dotimer();
private:
    Timermgr();
    ~Timermgr();

private:
    TIMER_MAP m_mapTimer;
    TIMER_WHOOL m_mapTimerWhool;
    tlib::TPool<TimerHandler, false> m_poolTimer;
};

#endif	/* __Timermgr_h__ */

