#ifndef __TPool_h__
#define __TPool_h__

#include "CLock.h"
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
    template <typename type, bool lock = false, s32 size = 4096>
    class TPool {
    public:
        typedef list<type *> UNIT_POOL;
        TPool() {
            for (s32 i=0; i<size; i++) {
                m_list.push_back(m_szUnit+i);
            }
            if (lock) {
                m_pLock = NEW CLockUnit;
                TASSERT(m_pLock, "new faild");
            }
        }

        ~TPool() {
            if (lock) {
                delete m_pLock;
            }
        }

        type * Create() {
            type * p = NULL;
            POOL_OPT_LOCK(lock, m_pLock);
    #ifdef _DEBUG
            static s32 test = 0;
            test ++;
    #endif //_DEBUG
            typename UNIT_POOL::iterator itor = m_list.begin();
            if (itor != m_list.end()) {
                p = *itor;
                m_list.erase(itor);
            }
    #ifdef _DEBUG
            test --;
            TASSERT(test == 0, "tpool has some bugs");
    #endif //_DEBUG
            POOL_OPT_FREELOCK(lock, m_pLock);
            return p;
        }

        bool Recover(type * pUnit) {
            if ( (char *)pUnit < (char *)m_szUnit 
                || (char *)pUnit > (char *)(m_szUnit + size - 1)
                || 0 != ((char *)pUnit - (char *)m_szUnit)%sizeof(type) ) {
                    TASSERT(false, "point adress error");
                    return false;
            }
            POOL_OPT_LOCK(lock, m_pLock);
    #ifdef _DEBUG
            static s32 test = 0;
            test ++;
    #endif //_DEBUG
            typename UNIT_POOL::iterator ibegin = m_list.begin();
            typename UNIT_POOL::iterator iend = m_list.end();
            typename UNIT_POOL::iterator itor = ::find(ibegin, iend, pUnit);
            if (itor != iend) {
    #ifdef _DEBUG
                test --;
                TASSERT(test == 0, "tpool has some bugs");
    #endif //_DEBUG
                POOL_OPT_FREELOCK(lock, m_pLock);
                return false;
            }
            m_list.push_back(pUnit);
    #ifdef _DEBUG
            test --;
            TASSERT(test == 0, "tpool has some bugs");
    #endif //_DEBUG
            POOL_OPT_FREELOCK(lock, m_pLock);
            return true;
        }

    private:
        UNIT_POOL m_list;
        type m_szUnit[size];
        CLockUnit * m_pLock;
    };
}

#endif //__TPool_h__
