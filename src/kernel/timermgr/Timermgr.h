/* 
 * File:   Timermgr.h
 * Author: alax
 *
 * Created on March 5, 2015, 11:02 AM
 */

#ifndef __Timermgr_h__
#define	__Timermgr_h__

#include "ITimermgr.h"
#include "TimeSlot.h"
#include <map>
class TimeNode;
typedef std::map<s32, TimeSlot*> TIMESLOT_MAP;

class Timermgr : public ITimermgr {
public:
    static Timermgr * getInstance(s32 scale = 20);

    virtual bool Redry();
    virtual bool Initialize();
    virtual bool Destory();
    virtual s64 Processing();

    // tiemr interface
    virtual bool StartTimer(s32 id, tcore::ITimer * timer, s64 interval, s64 delay, s64 loop);
    virtual bool KillTimer(s32 id, tcore::ITimer * timer);
    virtual bool PauseTimer(s32 id, tcore::ITimer * timer);
    virtual bool ResumeTimer(s32 id, tcore::ITimer * timer);

    bool ResetTimeNode(TimeNode * pNode);
private:
    void SlotAdd(s32 nCursor, TimeNode * pNode);
    void SlotRun(s32 nCursor, s64 lTick);

    Timermgr(s32 scale);
    ~Timermgr();

private:
    s64 m_lStartRunTick; //开始执行时间
    s32 m_nTimeScale; //时间刻度
    s32 m_nCursor; //游标
    TIMESLOT_MAP m_oTimeSlotMap; //时间槽
};

#endif	/* __Timermgr_h__ */

