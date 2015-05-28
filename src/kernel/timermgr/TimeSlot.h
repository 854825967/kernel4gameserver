#ifndef __TimeSlot_h__
#define __TimeSlot_h__

#include "MultiSys.h"
#include "TPool.h"

class TimeNode;

class TimeSlot {
public:
    static TimeSlot * GetTimeSlot() {
        return s_oTimeSlotPool.Create();
    }

    void Release() {
        m_pHead = NULL;
        m_nLen = 0;
        s_oTimeSlotPool.Recover(this);
    }

    void Add(TimeNode * pNode);
    void Run(const s64 lTick);

private:
    typedef tlib::TPool<TimeSlot, false, 1024> TIMESLOT_POOL;
    TimeSlot();
    friend class TIMESLOT_POOL;

private:
    static TIMESLOT_POOL s_oTimeSlotPool;

    TimeNode * m_pHead;
    s32 m_nLen;
};

#endif //__TimeSlot_h__
