#ifndef __iocpworker_h__
#define __iocpworker_h__

#include "iocphead.h"
#include "CThread.h"
#include "CHashMap.h"

class iocpworker : public CThread {
    typedef CHashMap<s64, ISocket *> SOCKET_CLIENT_RELATION;
public:
    iocpworker() : m_hCompletionPort(NULL) {

    }

    void InitCompletionPort(HANDLE hCompletionPort) {
        TASSERT(m_hCompletionPort == NULL, "wtf");
        m_hCompletionPort = hCompletionPort;
    }

    virtual void Run();

    virtual void Terminate() {
        if (THREAD_WORKING == m_nStatus) {
            m_nStatus = THREAD_STOPPING;
            while (m_nStatus != THREAD_STOPED) {
                CSLEEP(1);
            }
        }
    }

    s64 DealEvent(s64 overtime);

private:
    void SendDisconnectEvent(struct iocp_event * & pEvent);

    void RelateSocketClient(const s64 socket, ISocket * pClient);
    bool CheckSocketClient(const s64 socket);

private:
    HANDLE m_hCompletionPort;
    s8 m_nStatus;
    tlib::TQueue<struct iocp_event *, false, 4096> m_queueEvent;
    SOCKET_CLIENT_RELATION m_mapSocketClient;
};

#endif //__iocpworker_h__
