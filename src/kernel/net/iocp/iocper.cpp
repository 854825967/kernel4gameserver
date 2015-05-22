#include "iocper.h"
#include "Tools.h"
#include "Kernel.h"
#include "CLock.h"
#include "SPipe.h"
#include "CPipe.h"
#include "iocpworker.h"
#include "configmgr/Configmgr.h"
using namespace tcore;
using namespace tlib;

INetengine * iocper::getInstance() {
    static INetengine * p = NULL;
    if (!p) {
        p = NEW iocper;
        if (!p->Redry()) {
            TASSERT(false, "iocper can't be ready");
            delete p;
            p = NULL;
        }
    }

    return p;
}

bool iocper::Redry() {

    return true;
}

bool iocper::Initialize() {
    m_nWorkerCount = Configmgr::getInstance()->GetCoreConfig()->sNetThdCount;

    SetLastError(0);
    if (NULL == (g_pFunAcceptEx = GetAcceptExFun())) {
        TASSERT(false, "GetAcceptExFun error %d", ::GetLastError());
        return false;
    }

    if (NULL == (g_pFunConnectEx = GetConnectExFun())) {
        TASSERT(false, "GetConnectExFun error %d", ::GetLastError());
        return false;
    }

    if (NULL == (m_hCompletionPortAC = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0))) {
        TASSERT(false, "CreateIoCompletionPort error %d", ::GetLastError());
        return false;
    }

    m_pCompletionPortAry = NEW HANDLE[m_nWorkerCount];
    TASSERT(m_pCompletionPortAry, "wtf");

    for (s32 i=0; i<m_nWorkerCount; i++) {
        if (NULL == (m_pCompletionPortAry[i] = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0))) {
            TASSERT(false, "CreateIoCompletionPort error %d", ::GetLastError());
            return false;
        }
    }

    m_pIocpworkerAry = NEW iocpworker[m_nWorkerCount];
    for (s32 i=0; i<m_nWorkerCount; i++) {
        if (m_pIocpworkerAry[i].Initialize()) {
            m_pIocpworkerAry[i].Start();
        } else {
            TASSERT(false, "wtf");
        }
    }
    
    return true;
}

bool iocper::Destory() {
    for (s32 i=0; i<m_nWorkerCount; i++) {
        m_pIocpworkerAry[i].Terminate();
    }

    delete[] m_pIocpworkerAry;

    CloseHandle(m_hCompletionPortAC);
    delete this;
    return true;
}

bool iocper::AddClient(tcore::ITcpSession * client, const char * ip, const s32 port) {
    SetLastError(0);
    s64 lSockert = INVALID_SOCKET;
    CPipe * pCPipe = g_oCPipePool.Create(); // NEW CPipe;
    memset(&pCPipe->m_oAddr, 0, sizeof(pCPipe->m_oAddr));
    pCPipe->m_oAddr.sin_family = AF_INET;
    DWORD dwValue = 0;

    if (INVALID_SOCKET == (lSockert = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))
        || SOCKET_ERROR == bind(lSockert, (struct sockaddr *)&pCPipe->m_oAddr, sizeof(struct sockaddr_in))
        || SOCKET_ERROR == ioctlsocket(lSockert, FIONBIO, &dwValue)) {
            
        TASSERT(false, "AddClient error %d", ::GetLastError());
        client->OnConnectFailed(Kernel::getInstance());
        closesocket(lSockert);
        g_oCPipePool.Recover(pCPipe); // delete pCPipe;
        return false;
    }

    pCPipe->m_lSocketHandler = lSockert;
    pCPipe->m_pHost = client;
    iocpworker * pWorker = BalancingWork();
    if (pWorker->AddPIPE(pCPipe) && pCPipe->DoConnect(ip, port)) {
        client->m_pPipe = pCPipe;
    } else {
        client->OnConnectFailed(Kernel::getInstance());
        g_oCPipePool.Recover(pCPipe); // delete pCPipe;
        return false;
    }

    return true;
}

bool iocper::AddServer(tcore::ITcpServer * pServer, const char * ip, const s32 port) {
    SetLastError(0);
    s64 lSockert = INVALID_SOCKET;
    int len = BUFF_SIZE;

    if (INVALID_SOCKET == (lSockert = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED))
        || SOCKET_ERROR == setsockopt(lSockert, SOL_SOCKET, SO_SNDBUF, (char *)&len, sizeof(int))
        || SOCKET_ERROR == setsockopt(lSockert, SOL_SOCKET, SO_RCVBUF, (char *)&len, sizeof(int))) {
        TASSERT(false, "socket error %d", ::GetLastError());
        return false;
    }


    SPipe * pSPipe = NEW SPipe;
    tools::SafeMemset(&pSPipe->m_oAddr, sizeof(pSPipe->m_oAddr), 0, sizeof(pSPipe->m_oAddr));
    pSPipe->m_oAddr.sin_family = AF_INET;
    pSPipe->m_oAddr.sin_port = htons(port);
    if(INADDR_NONE == (pSPipe->m_oAddr.sin_addr.s_addr = inet_addr(ip))
        || SOCKET_ERROR == bind(lSockert, (struct sockaddr*)&(pSPipe->m_oAddr), sizeof(struct sockaddr_in))
        || listen(lSockert, 2048) == SOCKET_ERROR
        || m_hCompletionPortAC != (CreateIoCompletionPort((HANDLE)lSockert, (HANDLE)m_hCompletionPortAC, (u_long)lSockert, 0))) {

        closesocket(lSockert);
        delete pSPipe;
        TASSERT(false, "addserver error %d, ip %s", ::GetLastError(), ip);
        return false;
    }

    pSPipe->m_nStatus = SS_ESTABLISHED;
    pSPipe->m_lSocketHandler = lSockert;
    pSPipe->m_hCompletionPort = m_hCompletionPortAC;
    pSPipe->m_pHost = pServer;
    pServer->m_pPipe = pSPipe;

    if (!pSPipe->DoAccept()) {
        delete pSPipe;
        pServer->Error(Kernel::getInstance(), NULL);
        pServer->m_pPipe = NULL;
        return false;
    }

    return true;
}

void iocper::CompletedAccept() {
    DWORD nIOBytes = 0;
    BOOL nSucceed = false;
    s64 socket_handler = INVALID_SOCKET;
    struct iocp_event * pEvent = NULL;
    SetLastError(0);
    nSucceed = GetQueuedCompletionStatus(m_hCompletionPortAC, &nIOBytes, (PULONG_PTR)&socket_handler, (LPOVERLAPPED *)&pEvent, 10);

    if(NULL == pEvent) {
        return;
    }
    TASSERT(SO_ACCEPT == pEvent->opt, "wtf");

    pEvent->code = GetLastError();
    if (!nSucceed ) {
        if (WAIT_TIMEOUT == pEvent->code) {
            return;
        }
    }

    // accept
    SPipe * pSPipe = (SPipe *)pEvent->pContext;
    TASSERT(pSPipe, "wtf");

    CPipe * pCPipe = g_oCPipePool.Create(); // NEW CPipe;
    TASSERT(pCPipe, "wtf");
    s32 nLen = sizeof(struct sockaddr);

    s32 res = 0;

    if ( 0 != ( res = setsockopt(pEvent->socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (const char*) &pSPipe->m_lSocketHandler, sizeof(pSPipe->m_lSocketHandler)) )
        || SOCKET_ERROR == getpeername(pEvent->socket, (struct sockaddr*)&pEvent->remote, &nLen)) {
            ECHO("complete accept error %d", ::GetLastError());
            closesocket(pEvent->socket);
            g_oCPipePool.Recover(pCPipe); // delete pCPipe;
    } else {
        ITcpServer * pServer = (ITcpServer *)pSPipe->m_pHost;
        TASSERT(pServer, "wtf");
        ITcpSession * pClient = pServer->MallocConnection(Kernel::getInstance());
        TASSERT(pClient, "MallocConnection return a NULL point");
        if (pClient == NULL) {
            closesocket(pEvent->socket);
            g_oCPipePool.Recover(pCPipe); // delete pCPipe;
        } else {
            iocpworker * pWorker = BalancingWork();
            pCPipe->m_pHost = pClient;
            pClient->m_pPipe = pCPipe;
            pCPipe->m_lSocketHandler = pEvent->socket;
            if (pWorker->AddPIPE(pCPipe)) {
                pCPipe->m_nStatus = SS_ESTABLISHED;
                formartIocpevent(pCPipe->m_pRecvEvent, pCPipe, pCPipe->m_lSocketHandler, SO_ACCEPT);
                bool res = PostQueuedCompletionStatus(pCPipe->m_hCompletionPort, 0, pCPipe->m_lSocketHandler, (LPWSAOVERLAPPED)pCPipe->m_pRecvEvent);
                TASSERT(res, "PostQueuedCompletionStatus error %d", ::GetLastError());
                pClient->OnConnected(Kernel::getInstance());
            } else {
                pClient->OnConnectFailed(Kernel::getInstance());
            }
        }
    }

    if (!pSPipe->DoAccept()) {
        pSPipe->m_pHost->Error(Kernel::getInstance(), NULL);
        pSPipe->m_pHost->m_pPipe = NULL;
        delete pSPipe;
    }
}

iocpworker * iocper::BalancingWork() {
    static s32 index = 0;
    return &m_pIocpworkerAry[index ++ % Configmgr::getInstance()->GetCoreConfig()->sNetThdCount];
}

s64 iocper::Processing(s64 overtime) {
    s64 lTick = tools::GetTimeMillisecond();

    CompletedAccept();

    for (s32 i=0; i<m_nWorkerCount; i++) {
        m_pIocpworkerAry[i].Processing(overtime/4);
    }
    return tools::GetTimeMillisecond() - lTick;
}
