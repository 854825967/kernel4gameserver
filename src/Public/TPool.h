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

    template <typename type, bool lock = false, s32 size = 64, s32 count = 64>
    class TPool {
    public:
        enum {
            IN_USE,
            IS_FREE
        };

        struct type_info {
            type unit;
            const s32 len;
            s8 status;

            type_info() : len(sizeof(type_info)) {
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

            AllocNewBlob(size);
        }

        type * Create() {
            type * p = NULL;
            POOL_OPT_LOCK(lock, m_pLock);
            if (m_list.empty()) {
                AllocNewBlob(count);
            }

            typename UNIT_POOL::iterator itor = m_list.begin();

            if (itor != m_list.end()) {
                p = (type *)*itor;
                TASSERT((*itor)->status == IS_FREE && (*itor)->len == sizeof(type_info), "wtf");
                (*itor)->status = IN_USE;
                m_list.erase(itor);
            }

            POOL_OPT_FREELOCK(lock, m_pLock);
            return p;
        }

        bool Recover(type * pUnit) {
            if (!CheckAddr(pUnit) || ((type_info *)pUnit)->status != IN_USE) {
                TASSERT(false, "point adress error");
                return false;
            }
            POOL_OPT_LOCK(lock, m_pLock);

//             typename UNIT_POOL::iterator ibegin = m_list.begin();
//             typename UNIT_POOL::iterator iend = m_list.end();
//             typename UNIT_POOL::iterator itor = ::find(ibegin, iend, (type_info *)pUnit);
//             if (itor != iend) {
//                 POOL_OPT_FREELOCK(lock, m_pLock);
//                 return false;
//             }
            ((type_info *)pUnit)->status = IS_FREE;
            m_list.push_back((type_info *)pUnit);

            POOL_OPT_FREELOCK(lock, m_pLock);
            return true;
        }

        ~TPool() {
            for (s32 i = 0; i < m_sBlockCount; i++) {
                delete[] m_pUnitAry[i];
            }
            if (m_sBlockCount > 1) {
                delete m_pUnitAry;
            } else {
                delete[] m_pUnitAry;
            }

            if (lock) {
                delete m_pLock;
            }
        }
    private:
        void AllocNewBlob(s32 _count) {
            if (m_pUnitAry == NULL) {
                TASSERT(m_sBlockCount == 0, "pool bug");
                m_pUnitAry = NEW type_info*;
            } else {
                type_info ** p = NEW type_info*[m_sBlockCount + 1];
                tools::SafeMemcpy(p, (m_sBlockCount + 1) * sizeof(type_info *),m_pUnitAry, m_sBlockCount * sizeof (type_info *));
                if (1 == m_sBlockCount) {
                    delete m_pUnitAry;
                } else {
                    delete[] m_pUnitAry;
                }

                m_pUnitAry = p;
            }

            m_pUnitAry[m_sBlockCount] = NEW type_info[_count];
            for (s32 i = 0; i < _count; i++) {
                m_list.push_back(m_pUnitAry[m_sBlockCount] + i);
            }
            m_sBlockCount++;
        }

        bool CheckAddr(type * pUnit) {
            return ((type_info *)pUnit)->len == sizeof(type_info);
        }

    private:
        UNIT_POOL m_list;
        s32 m_sBlockCount;
        type_info ** m_pUnitAry;
        CLockUnit * m_pLock;
    };
}

#endif //__TPool_h__
