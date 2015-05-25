#ifndef __CLock_h__
#define __CLock_h__

#include "CLockUnit.h"

namespace tlib {
    class CLock {
    public:
        CLock(CLockUnit * pLock) {
            TASSERT(pLock != NULL, "CLockUnit point null");
            m_pLock = pLock;
            m_pLock->Lock();
        }

        void Free() {
            TASSERT(m_pLock != NULL, "CLockUnit point null");
            m_pLock->UnLock();
        }

        ~CLock() {
            m_pLock->UnLock();
        }

    private:
        CLockUnit * m_pLock;
    };
}

#endif //defined __CLock_h__

