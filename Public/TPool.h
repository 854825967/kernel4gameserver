#ifndef __TPool_h__
#define __TPool_h__

#include "CLock.h"
#include "Tools.h"
#include <list>
#include <algorithm>
using namespace std;

#define POOL_OPT_LOCK(b, lock) \
    if (b) { \
        lock->Lock(); \
    }
#define POOL_OPT_FREELOCK(b, lock) \
    if (b) { \
        lock->UnLock(); \
    }
namespace tlib {

    template <typename type, bool lock = false, s32 size = 64 >
    class TPool {
    public:
        enum {
            IN_USE,
            IS_FREE
        };

        struct type_info {
            type unit;
            const s32 size;
            s8 status;

            type_info() : size(sizeof(type_info)) {
                status = IS_FREE;
            }

        };
        typedef list<type_info *> UNIT_POOL;

        TPool() {
            m_pUnitAry = 0;
            m_sBlockCount = 0;
            if (lock) {
                m_pLock = NEW CLockUnit;
                TASSERT(m_pLock, "new faild");
            }

            AllocNewBlob();
        }

        ~TPool() {
            for (s32 i = 0; i < m_sBlockCount; i++) {
                delete[] m_pUnitAry[i];
            }

            delete m_pUnitAry;

            if (lock) {
                delete m_pLock;
            }
        }

        type * Create() {
#ifdef _DEBUG
            s64 lTick = tools::GetTimeMillisecond();
#endif //_DEBUG
            type * p = NULL;
            POOL_OPT_LOCK(lock, m_pLock);
#ifdef _DEBUG
            static s32 test = 0;
            test++;
#endif //_DEBUG
            if (m_list.empty()) {
                AllocNewBlob();
            }

            typename UNIT_POOL::iterator itor = m_list.begin();

            if (itor != m_list.end()) {
                p = (type *)*itor;
                TASSERT((*itor)->status == IS_FREE && (*itor)->size == sizeof(type_info), "wtf");
                (*itor)->status = IN_USE;
                m_list.erase(itor);
            }
#ifdef _DEBUG
            test--;
            TASSERT(test == 0, "tpool has some bugs");
#endif //_DEBUG
            POOL_OPT_FREELOCK(lock, m_pLock);
#ifdef _DEBUG
            s64 lUse = tools::GetTimeMillisecond() - lTick;
            if (lUse > 5) {
                ECHO("Pool create use tick %ld", lUse);
            }
#endif //_DEBUG
            return p;
        }

        bool Recover(type * pUnit) {
#ifdef _DEBUG
            s64 lTick = tools::GetTimeMillisecond();
#endif //_DEBUG
            if (!CheckAddr(pUnit) || ((type_info *)pUnit)->status != IN_USE) {
                TASSERT(false, "point adress error");
                return false;
            }
            POOL_OPT_LOCK(lock, m_pLock);
#ifdef _DEBUG
            static s32 test = 0;
            test++;
#endif //_DEBUG
            typename UNIT_POOL::iterator ibegin = m_list.begin();
            typename UNIT_POOL::iterator iend = m_list.end();
            typename UNIT_POOL::iterator itor = ::find(ibegin, iend, (type_info *)pUnit);
            if (itor != iend) {
#ifdef _DEBUG
                test--;
                TASSERT(test == 0, "tpool has some bugs");
#endif //_DEBUG
                POOL_OPT_FREELOCK(lock, m_pLock);
                return false;
            }
            ((type_info *)pUnit)->status = IS_FREE;
            m_list.push_back((type_info *)pUnit);
#ifdef _DEBUG
            test--;
            TASSERT(test == 0, "tpool has some bugs");
#endif //_DEBUG
            POOL_OPT_FREELOCK(lock, m_pLock);
#ifdef _DEBUG
            s64 lUse = tools::GetTimeMillisecond() - lTick;
            if (lUse > 5) {
                ECHO("Pool Recover use tick %ld", lUse);
            }
#endif //_DEBUG
            return true;
        }
    private:

        void AllocNewBlob() {
            POOL_OPT_LOCK(lock, m_pLock);
            if (m_pUnitAry == NULL) {
                TASSERT(m_sBlockCount == 0, "pool bug");
                m_pUnitAry = NEW type_info*;
            } else {
                type_info ** p = NEW type_info*[m_sBlockCount + 1];
                tools::SafeMemcpy(p, (m_sBlockCount + 1) * sizeof(type_info *),m_pUnitAry, m_sBlockCount * sizeof (type_info *));
                delete[] m_pUnitAry;
                m_pUnitAry = p;
            }


            m_pUnitAry[m_sBlockCount] = NEW type_info[size];
            for (s32 i = 0; i < size; i++) {
                m_list.push_back(m_pUnitAry[m_sBlockCount] + i);
            }
            m_sBlockCount += 1;
            POOL_OPT_FREELOCK(lock, m_pLock);
        }

        bool CheckAddr(type * pUnit) {
            return ((type_info *)pUnit)->size == sizeof(type_info);
        }

    private:
        UNIT_POOL m_list;
        s32 m_sBlockCount;
        type_info ** m_pUnitAry;
        CLockUnit * m_pLock;
    };
}

#endif //__TPool_h__
