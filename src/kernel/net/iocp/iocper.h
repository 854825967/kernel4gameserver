#ifndef __iocper_h__
#define __iocper_h__

#include "iocphead.h"
class iocpworker;

class iocper : public INetengine {
public:
    static INetengine * getInstance();

    virtual bool Redry();
    virtual bool Initialize();
    virtual bool Destory();

    virtual bool AddServer(tcore::ITcpServer * server, const char * ip, const s32 port);
    virtual bool AddClient(tcore::ITcpSession * client, const char * ip, const s32 port);
    virtual s64 Processing(s64 overtime);


private:
    iocper() {
        m_pCompletionPortAry = NULL;
        m_nWorkerCount = 0;
    }

    void CompletedAccept();
    iocpworker * BalancingWork();

private:
    HANDLE * m_pCompletionPortAry;
    iocpworker * m_pIocpworkerAry;

    HANDLE m_hCompletionPortAC; //Accept & Connect
    s32 m_nWorkerCount;
    tlib::TPool<CPipe> m_oCPipePool;
    tlib::CLockUnit m_threadLock;
};


#endif //__iocper_h__
