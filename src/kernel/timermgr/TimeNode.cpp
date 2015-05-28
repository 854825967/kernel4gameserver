#include "TimeNode.h"
#include "Timermgr.h"

TIMENODE_MAP TimeNode::s_oTimeNodeMap;
TPool<TimeNode> TimeNode::s_oTimeNodePool;

TimeNode * TimeNode::FindNode(s32 id, tcore::ITimer * timer, bool remove /*= false*/) {
    TimeNode * pNode = NULL;
    TIMENODE_MAP::iterator itor = s_oTimeNodeMap.find(nodeinfo(id, timer));
    if (itor != s_oTimeNodeMap.end()) {
        TASSERT(timer == itor->second->GetTimer(), "wtf");
        pNode = itor->second;
        pNode->SetStatus(TIMER_STATUS_CLOSE);
        if (remove) {
            s_oTimeNodeMap.erase(itor);
        }
    }
    return pNode;
}

void TimeNode::Initialize(s32 id, tcore::ITimer * timer, s32 interval, s32 loop, s32 delay /*= 0*/, TimeNode * pNext /* = NULL */) {
    tools::SafeMemset(this, sizeof(*this), 0, sizeof(TimeNode));
    m_byStatus = TIMER_STATUS_RUN;
    m_nID = id;
    m_pTimer = timer;
    m_nInterval = interval;
    m_nMaxHitCount = loop;
    m_pNext = pNext;
    m_lNextRunTick = tools::GetTimeMillisecond() + delay;
}

TimeNode * TimeNode::SetTimer(s32 id, tcore::ITimer * timer, s32 interval, s32 loop/* = LOOP_FOREVER*/, s32 delay/* = 0*/) {
    TimeNode * pNode = FindNode(id, timer, true);
    if (pNode != NULL) {
        ECHO_TRACE("timer id %d mem %lx reset interval %d, loop %d", id, timer, interval, loop);
    }

    pNode = s_oTimeNodePool.Create();
    pNode->Initialize(id, timer, interval, loop, delay);

    return pNode;
}

void TimeNode::OnTimer(const s64 lTick) {
    if (0 == m_nHitCount) {
        m_pTimer->OnStart(Kernel::getInstance(), m_nID, lTick);
    }

    m_pTimer->OnTime(Kernel::getInstance(), m_nID, lTick);
    m_lNextRunTick += m_nInterval;
    m_nHitCount ++;
    if (m_nHitCount == m_nMaxHitCount && m_nMaxHitCount != LOOP_FOREVER) {
        m_pTimer->OnTerminate(Kernel::getInstance(), m_nID, true, lTick);
        FindNode(m_nID, m_pTimer, true);
        Release();
    } else {
        Timermgr::getInstance()->ResetTimeNode(this);
    }
}

void TimeNode::Stop() {
    
}

void TimeNode::Pause() {

}

void TimeNode::Resume() {

}

TimeNode::TimeNode() {

}

TimeNode::~TimeNode() {

}
