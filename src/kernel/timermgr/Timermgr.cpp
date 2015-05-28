/* 
 * File:   Timermgr.cpp
 * Author: alax
 * 
 * Created on March 5, 2015, 11:02 AM
 */
#include "Timermgr.h"
#include "TimeNode.h"

Timermgr::Timermgr(s32 scale) {
    m_nTimeScale = scale;
}

Timermgr::~Timermgr() {
}

Timermgr * Timermgr::getInstance(s32 scale){
    static Timermgr * pTimermgr = NULL;
    if (NULL == pTimermgr) {
        pTimermgr = NEW Timermgr(scale);
        TASSERT(pTimermgr, "wtf");
    }

    return pTimermgr;
}

bool Timermgr::Redry() {
    return true;
}

bool Timermgr::Initialize() {
    m_nCursor = 0; //游标
    m_lStartRunTick = tools::GetTimeMillisecond(); //开始执行时间
    return true;
}

bool Timermgr::Destory() {
    delete this;
    return true;
}

// tiemr interface 
bool Timermgr::StartTimer(s32 id, tcore::ITimer * timer, s64 interval, s64 delay, s64 loop) {
    TASSERT(interval >= 100, "wtf");
    TimeNode * pNode = TimeNode::SetTimer(id, timer, interval, loop, delay);
    ResetTimeNode(pNode);
    return true;
}

void Timermgr::SlotAdd(s32 nCursor, TimeNode * pNode) {
    TIMESLOT_MAP::iterator itor = m_oTimeSlotMap.find(nCursor);
    if (itor == m_oTimeSlotMap.end()) {
        itor = m_oTimeSlotMap.end();
        itor = m_oTimeSlotMap.insert(itor, TIMESLOT_MAP::value_type(nCursor, TimeSlot::GetTimeSlot()));
    }
    itor->second->Add(pNode);
}

void Timermgr::SlotRun(s32 nCursor, s64 lTick) {
    TIMESLOT_MAP::iterator itor = m_oTimeSlotMap.find(nCursor);
    if (itor != m_oTimeSlotMap.end()) {
        itor->second->Run(lTick);
        itor->second->Release();
        m_oTimeSlotMap.erase(itor);
    }
}

bool Timermgr::KillTimer(s32 id, tcore::ITimer * timer) {
    if (TimeNode::FindNode(id, timer, true) == NULL) {
        ECHO_ERROR("timer %lx id %d not exist", timer, id);
    }

    return true;
}

bool Timermgr::PauseTimer(s32 id, tcore::ITimer * timer) {
    return false;
}

bool Timermgr::ResumeTimer(s32 id, tcore::ITimer * timer) {
    return false;
}

bool Timermgr::ResetTimeNode(TimeNode * pNode) {
    TASSERT(pNode->GetStatus() == TIMER_STATUS_RUN, "wtf");
    s32 nOffset = (pNode->GetNextRunTick() - m_lStartRunTick)/m_nTimeScale;
    SlotAdd(nOffset, pNode);
    return true;
}

s64 Timermgr::Processing() {
    s64 lTick = tools::GetTimeMillisecond();
    s32 nTick = lTick - m_lStartRunTick;
    s32 nCursor = (nTick/m_nTimeScale);
    for (; m_nCursor < nCursor; m_nCursor++) {
        SlotRun(m_nCursor, lTick);
    }

    return tools::GetTimeMillisecond() - lTick;
}
