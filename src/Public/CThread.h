#ifndef CTHREAD_H
#define CTHREAD_H

#include "MultiSys.h"

namespace tlib {
    enum {
        THREAD_STOPED = 0,
        THREAD_WORKING,
        THREAD_STOPPING,
    };

    class CThread
    {
    public:
        CThread();
        virtual ~CThread();

        //interface
    public:
        bool Start(s32 threadcount = 1);
        bool Wait();

        bool Terminate(); // not safe
        virtual void Run() = 0;

        inline unsigned long long GetThread() {return m_hThread;};

    protected:
        s8 m_nStatus;
    private:
        unsigned long long m_hThread;
    };
}

#endif //define CTHREAD_H
