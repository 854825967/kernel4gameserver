#ifndef __iocpworker_h__
#define __iocpworker_h__

#include "iocphead.h"
#include "ICore.h"
#include "CThread.h"
#include "CHashMap.h"

class iocpworker : public CThread {
    typedef CHashMap<s64, CPipe *> SOCKET_CLIENT_RELATION;
    typedef TQueue<ioevent *, false, 4096> IOEVENT_QUEUE;
    typedef TPool<ioevent, true> IOEVENT_POOL;
public:
    iocpworker();
    virtual void Run();
    virtual void Terminate();
    s64 Processing(s64 overtime);

    bool Initialize();
    inline HANDLE GetCompletionPort() {
        return m_hCompletionPort;
    }

    bool AddPIPE(CPipe * pCPipe);

private:
    void SendDisconnectEvent(const s64 socket, CPipe * pClient);
    void RelateSocketClient(const s64 socket, CPipe * pCPipe);
    bool CheckSocketClient(const s64 socket, CPipe * pCPipe);

private:
    HANDLE m_hCompletionPort;
    SOCKET_CLIENT_RELATION m_mapSocketClient;
    IOEVENT_QUEUE m_oEventQueue;
    IOEVENT_POOL m_oEventPool;
};

#endif //__iocpworker_h__
