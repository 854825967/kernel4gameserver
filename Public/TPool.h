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
        typedef list<type *> UNIT_POOL;

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
                p = *itor;
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
            if (!CheckAddr(pUnit)) {
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
            typename UNIT_POOL::iterator itor = ::find(ibegin, iend, pUnit);
            if (itor != iend) {
#ifdef _DEBUG
                test--;
                TASSERT(test == 0, "tpool has some bugs");
#endif //_DEBUG
                POOL_OPT_FREELOCK(lock, m_pLock);
                return false;
            }
            m_list.push_back(pUnit);
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
                m_pUnitAry = NEW type*;
            } else {
                type ** p = NEW type*[m_sBlockCount + 1];
                tools::SafeMemcpy(p, (m_sBlockCount + 1) * sizeof(type *),m_pUnitAry, m_sBlockCount * sizeof (type *));
                delete[] m_pUnitAry;
                m_pUnitAry = p;
            }


            m_pUnitAry[m_sBlockCount] = NEW type[size];
            for (s32 i = 0; i < size; i++) {
                m_list.push_back(m_pUnitAry[m_sBlockCount] + i);
            }
            m_sBlockCount += 1;
            POOL_OPT_FREELOCK(lock, m_pLock);
        }

        bool CheckAddr(type * pUnit) {
            for (s32 i = 0; i < m_sBlockCount; i++) {
                if (pUnit >= &(m_pUnitAry[i][0])
                        && pUnit <= &(m_pUnitAry[i][size - 1])
                        && 0 == ((char *)pUnit - (char *)&(m_pUnitAry[i][0])) % sizeof (type)) {

                    return true;
                }
            }
            
            return false;
        }

    private:
        UNIT_POOL m_list;
        s32 m_sBlockCount;
        type ** m_pUnitAry;
        CLockUnit * m_pLock;
    };
}

#endif //__TPool_h__
