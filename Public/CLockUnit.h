#ifndef __CLockUnit_h__
#define __CLockUnit_h__

#include "MultiSys.h"

#if defined WIN32
namespace tlib{
    class CLockUnit {
    public:
        CLockUnit() {
            ::InitializeCriticalSection(&m_lock);
        }

        ~CLockUnit() {
            ::DeleteCriticalSection(&m_lock);
        }

        void Lock() {
            ::EnterCriticalSection(&m_lock);
        }

        void UnLock() {
            ::LeaveCriticalSection(&m_lock);
        }

        bool TryLock() {
            ::TryEnterCriticalSection(&m_lock);
        }

    private:
        CRITICAL_SECTION m_lock;
    };
}
#else
// linux
namespace tlib {

    class CLockUnit {
    public:

        CLockUnit() {
            pthread_mutex_init(&m_lock, NULL);
        }

        ~CLockUnit() {
            pthread_mutex_destroy(&m_lock);
        }

        void Lock() {
            pthread_mutex_lock(&m_lock);
        }

        void UnLock() {
            pthread_mutex_unlock(&m_lock);
        }

        bool TryLock() {
            return !pthread_mutex_trylock(&m_lock);
        }

    private:
        pthread_mutex_t m_lock;
    };
}
#endif //defined WIN32 || defined WIN64


#endif //defined  __CLockUnit_h__

