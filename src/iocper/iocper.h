#ifndef __iocper_h__
#define __iocper_h__

#include "iocphead.h"
#include "iocpworker.h"
#include "CThread.h"

class iocper : public INetengine {
public:
    static INetengine * getInstance();

    virtual bool Redry();
    virtual bool Initialize();
    virtual bool Destory();

    virtual bool AddServer(tcore::ITcpServer * server);
    virtual bool AddClient(tcore::ITcpSocket * client);
    virtual s64 DonetIO(s64 overtime);

    inline HANDLE getCompletionPortByIndex(s32 index) {
        return m_pCompletionPortAry[index];
    }

private:
    iocper() {
        m_pCompletionPortAry = NULL;
        m_nWorkerCount = 0;
    }

    void CompletedAccept();
    HANDLE BalancingCompletionPort(HANDLE hCompletionPort = NULL);

private:
    HANDLE * m_pCompletionPortAry;
    iocpworker * m_pIocpworkerAry;

    HANDLE m_hCompletionPortAC; //Accept & Connect
    s32 m_nWorkerCount;

    tlib::CLockUnit m_threadLock;
};


#endif //__iocper_h__
