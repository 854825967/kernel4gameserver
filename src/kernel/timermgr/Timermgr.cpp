/* 
 * File:   Timermgr.cpp
 * Author: alax
 * 
 * Created on March 5, 2015, 11:02 AM
 */

#include "Timermgr.h"
#include "Tools.h"
#include "TimerHandler.h"

Timermgr::Timermgr() {

}

Timermgr::~Timermgr() {

}

ITimermgr * Timermgr::getInstance() {
    static Timermgr * p = NULL;
    if (NULL == p) {
        p = NEW Timermgr;
        if (!p->Redry()) {
            TASSERT(false, "Timermgr cant ready");
            delete p;
            p = NULL;
        }
    }

    return p;
}

bool Timermgr::Redry() {
    return true;
}

bool Timermgr::Initialize() {
    return true;
}

bool Timermgr::Destory() {
    return true;
}

// tiemr interface 

bool Timermgr::StartTimer(s32 id, tcore::ITimer * timer, s64 interval, s64 delay, s64 loop) {
    TASSERT(timer && loop != LOOP_OVER && interval >= 100 && delay >= 0, "check ur timer args");
    s64 lTick = tools::GetTimeMillisecond();
    {
        TIMER_MAP::iterator itor = m_mapTimer.find(timer_index(id, timer));
        if (itor != m_mapTimer.end()) {
            TASSERT(false, "timer id has been rgs");
            return false;
        }
    }

    TimerHandler * pHandler = m_poolTimer.Create();
    pHandler->m_sID = id;
    pHandler->m_sInterval = interval;
    pHandler->m_sNextRunTick = lTick + interval + delay;
    pHandler->m_sStatus = TIMER_STATUS_RUN;
    pHandler->m_sLoop = loop;
    pHandler->m_pTimer = timer;

    {
        TIMER_WHOOL::iterator itor = m_mapTimerWhool.find(interval);
        if (itor == m_mapTimerWhool.end()) {
            m_mapTimerWhool.insert(make_pair(interval, TIMER_LIST()));
            itor = m_mapTimerWhool.find(interval);
        }

        if (itor->second.empty()) {
            itor->second.push_back(pHandler);
        } else {
            TIMER_LIST::iterator list_itor = itor->second.begin();
            TIMER_LIST::iterator list_iend = itor->second.end();
            while (list_itor != list_iend) {
                if (pHandler->m_sNextRunTick > (*list_itor)->m_sNextRunTick) {
                    list_itor++;
                } else {
                    itor->second.insert(list_itor, pHandler);
                    break;
                }
            }
            
            if (list_itor == list_iend) {
                itor->second.push_back(pHandler);
            }
        }

    }

    m_mapTimer.insert(make_pair(timer_index(id, timer), pHandler));
    timer->OnStart(Kernel::getInstance(), id, lTick);

    return true;
}

bool Timermgr::KillTimer(s32 id, tcore::ITimer * timer) {
    s64 now = tools::GetTimeMillisecond();
    TIMER_MAP::iterator itor = m_mapTimer.find(timer_index(id, timer));
    if (itor == m_mapTimer.end()) {
        TASSERT(false, "timer id dosnt exist");
        return false;
    }

    itor->second->m_sStatus = TIMER_STATUS_WAITING_REMOVE;
    itor->second->m_pTimer->OnTerminate(Kernel::getInstance(), itor->second->m_sID, false, now);
    m_mapTimer.erase(itor);
    return true;
}

bool Timermgr::KillTimer(tcore::ITimer * timer) {
    s64 now = tools::GetTimeMillisecond();
    TIMER_MAP::iterator itor = m_mapTimer.begin();
    TIMER_MAP::iterator iend = m_mapTimer.end();
    while (itor != iend) {
        if (itor->second->m_pTimer == timer) {
            itor->second->m_sStatus = TIMER_STATUS_WAITING_REMOVE;
            itor->second->m_pTimer->OnTerminate(Kernel::getInstance(), itor->second->m_sID, false, now);
            m_mapTimer.erase(itor++);
        } else {
            itor++;
        }
    }

    return true;
}

s64 Timermgr::Dotimer() {
    s64 lTick = tools::GetTimeMillisecond();

    TIMER_WHOOL::iterator itor = m_mapTimerWhool.begin();
    TIMER_WHOOL::iterator iend = m_mapTimerWhool.end();
    while (itor != iend) {
        {
            TIMER_LIST::iterator ilist_itor = itor->second.begin();
            TIMER_LIST::iterator ilist_iend = itor->second.end();
            while (ilist_itor != ilist_iend) {
                TimerHandler * pHandler = *ilist_itor;
                if (pHandler->IsRunTime(lTick)) {
                    pHandler->OnTimer(lTick);

                    itor->second.erase(ilist_itor++);
                    if (pHandler->m_sStatus != TIMER_STATUS_WAITING_REMOVE) {
                        itor->second.push_back(pHandler);
                        ilist_iend = itor->second.end();
                    }
                } else if (pHandler->m_sStatus == TIMER_STATUS_WAITING_REMOVE) {
                    itor->second.erase(ilist_itor++);
                } else {
                    break;
                }
            }
        }

        itor++;
    }


    return tools::GetTimeMillisecond() - lTick;
}
