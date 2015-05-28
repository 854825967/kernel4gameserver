#include "TimeSlot.h"
#include "TimeNode.h"

TimeSlot::TIMESLOT_POOL TimeSlot::s_oTimeSlotPool;

TimeSlot::TimeSlot() {
    m_pHead = NULL;
    m_nLen = 0; 
}

void TimeSlot::Add(TimeNode * pNode) {
    m_nLen ++;
    if (NULL == m_pHead) {
        m_pHead = pNode;
        pNode->SetNext(NULL);
    } else {
        pNode->SetNext(m_pHead);
        m_pHead = pNode;
    }
}

void TimeSlot::Run(const s64 lTick) {
    while (m_pHead != NULL) {
        TimeNode * pNode = m_pHead;
        m_pHead = m_pHead->GetNext();
        switch (pNode->GetStatus()) {
        case TIMER_STATUS_RUN:
            pNode->OnTimer(lTick);
            break;
        case TIMER_STATUS_PAUSE:
            break;
        case TIMER_STATUS_CLOSE:
            pNode->Release();
            break;
        default:
            TASSERT(false, "wtf");
            break;
        }
    }
    m_nLen = 0;
}
