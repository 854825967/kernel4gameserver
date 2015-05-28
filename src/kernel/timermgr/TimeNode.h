/* 
 * File:   TimeNode.h
 * Author: alax
 *
 * Created on March 5, 2015, 1:55 PM
 */

#ifndef __TimeNode_h__
#define	__TimeNode_h__

#include "Kernel.h"
#include "TPool.h"
#include "CHashMap.h"
using namespace tlib;

enum TimerStatus {
    TIMER_STATUS_NOT_FOUND = 0,
    TIMER_STATUS_RUN,
    TIMER_STATUS_PAUSE,
    TIMER_STATUS_CLOSE
};

enum {
    LOOP_FOREVER = -1,
    LOOP_OVER = 0
};
class TimeNode;
class nodeinfo {
public:
    nodeinfo(s32 _id, tcore::ITimer * _p) {
        id = _id;
        p = _p;
    }

    operator size_t() const {
        return id + (s64)p;
    }

    tcore::ITimer * p;
    s32 id;
};
typedef CHashMap<nodeinfo, TimeNode *> TIMENODE_MAP;

class TimeNode {
public:
    static TimeNode * FindNode(s32 id, tcore::ITimer * timer, bool remove = false);
    static TimeNode * SetTimer(s32 id, tcore::ITimer * timer, s32 interval, s32 loop = LOOP_FOREVER, s32 delay = 0);

    inline void Release() {
        tools::SafeMemset(this, sizeof(*this), 0, sizeof(*this));
        s_oTimeNodePool.Recover(this);
    }

    void OnTimer(const s64 lTick);
    void Stop();
    void Pause();
    void Resume();

    inline s8 GetStatus() const {return m_byStatus;}
    inline void SetStatus(s8 status) {m_byStatus = status;}

    inline s32 GetID() const {return m_nID;}
    inline s32 GetMaxHitCount() const {return m_nMaxHitCount;}
    inline s32 GetHitCount() const {return m_nHitCount;}
    inline const tcore::ITimer * GetTimer() const {return m_pTimer;}

    inline TimeNode * GetNext() {return m_pNext;}
    inline void SetNext(TimeNode * pNext) {m_pNext = pNext;}

    inline s64 GetNextRunTick() {return m_lNextRunTick;}
    
private:
    friend class TPool<TimeNode>;
    TimeNode();
    ~TimeNode();

    void Initialize(s32 id, tcore::ITimer * timer, s32 interval, s32 loop, s32 delay = 0, TimeNode * pNext = NULL);

private:
    s8 m_byStatus;
    s32 m_nIndex;
    s32 m_nID;
    s32 m_nInterval;
    s32 m_nHitCount;
    s32 m_nMaxHitCount;
    s64 m_lNextRunTick; //下次执行时间

    tcore::ITimer * m_pTimer;
    TimeNode * m_pNext;

    static TIMENODE_MAP s_oTimeNodeMap;
    static TPool<TimeNode> s_oTimeNodePool;
};

#endif	/* __TimerHandler_h__ */

