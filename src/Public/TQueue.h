#ifndef __TQueue_h__
#define __TQueue_h__
#include "MultiSys.h"
#include "CLock.h"
#include "Tools.h"
#include <stdio.h>
#include <string.h>

#define QUEUE_OPT_LOCK(b, lock) \
    if (b) { \
    lock->Lock(); \
    }
#define QUEUE_OPT_FREELOCK(b, lock) \
    if (b) { \
    lock->UnLock(); \
    }

enum {
    NOT_EXISTS_DATA = 0,
    EXISTS_DATA = 1
};

namespace tlib {
    template <typename type, bool lock, const s32 size>
    class TQueue {
    public:
        TQueue() {
            m_nRIndex = 0;
            m_nWIndex = 0;
            m_nRCount = 0;
            m_nWCount = 0;
            if (lock) {
                m_pRlock = NEW CLockUnit;
                m_pWlock = NEW CLockUnit;
                TASSERT(m_pRlock && m_pWlock, "malloc lock mem faild");
            }

            memset(m_sign, 0, sizeof(m_sign));
        }

        ~TQueue() {
            if (lock) {
                delete m_pWlock;
                delete m_pRlock;
            }
        }

        inline void Add(type src) {
#ifdef _DEBUG
            s64 lTick = tools::GetTimeMillisecond();
#endif //_DEBUG
            QUEUE_OPT_LOCK(lock, m_pWlock);
            while (m_sign[m_nWIndex] != NOT_EXISTS_DATA) {
                CSLEEP(1);
            }

            m_queue[m_nWIndex] = src;
            m_sign[m_nWIndex++] = EXISTS_DATA;
            m_nWCount++;
            if (m_nWIndex >= size) {
                m_nWIndex = 0;
            }
            QUEUE_OPT_FREELOCK(lock, m_pWlock);
#ifdef _DEBUG
            s64 lUse = tools::GetTimeMillisecond() - lTick;
            if (lUse > 1) {
                ECHO("Queue Add use tick %ld", lUse);
            }
#endif //_DEBUG
        }

        inline bool TryAdd(type src) {
#ifdef _DEBUG
            s64 lTick = tools::GetTimeMillisecond();
#endif //_DEBUG
            QUEUE_OPT_LOCK(lock, m_pWlock);
            while (m_sign[m_nWIndex] != NOT_EXISTS_DATA) {
                QUEUE_OPT_FREELOCK(lock, m_pWlock);
                return false;
            }

            m_queue[m_nWIndex] = src;
            m_sign[m_nWIndex++] = EXISTS_DATA;
            m_nWCount++;
            if (m_nWIndex >= size) {
                m_nWIndex = 0;
            }
            QUEUE_OPT_FREELOCK(lock, m_pWlock);
#ifdef _DEBUG
            s64 lUse = tools::GetTimeMillisecond() - lTick;
            if (lUse > 1) {
                ECHO("Queue TryAdd use tick %ld", lUse);
            }
#endif //_DEBUG
            return true;
        }

        inline bool Read(type & value) {
#ifdef _DEBUG
            s64 lTick = tools::GetTimeMillisecond();
#endif //_DEBUG
            QUEUE_OPT_LOCK(lock, m_pRlock);
            while (m_sign[m_nRIndex] != EXISTS_DATA) {
                QUEUE_OPT_FREELOCK(lock, m_pRlock);
                return false;
            }

            value = m_queue[m_nRIndex];
            m_sign[m_nRIndex++] = NOT_EXISTS_DATA;
            m_nRCount++;

            if (m_nRIndex >= size) {
                m_nRIndex = 0;
            }
            QUEUE_OPT_FREELOCK(lock, m_pRlock);
#ifdef _DEBUG
            s64 lUse = tools::GetTimeMillisecond() - lTick;
            if (lUse > 1) {
                ECHO("Queue Read use tick %ld", lUse);
            }
#endif //_DEBUG
            return true;
        }

        inline bool IsEmpty() {
            return (m_nRCount == m_nWCount);
        }

    private:
        CLockUnit * m_pWlock;
        CLockUnit * m_pRlock;
        type m_queue[size];
        s8 m_sign[size];
        u32 m_nRIndex;
        u32 m_nWIndex;
        u32 m_nRCount;
        u32 m_nWCount;
    };
}

#endif //__TQueue_h__

