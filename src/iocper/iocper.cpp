#include "iocper.h"
#include "Tools.h"
#include "Kernel.h"
#include "configmgr/Configmgr.h"
#include "CLock.h"
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
        m_pIocpworkerAry[i].InitCompletionPort(m_pCompletionPortAry[i]);
        m_pIocpworkerAry[i].Start();
    }
    
    return true;
}

bool iocper::Destory() {

    return false;
}

bool iocper::AddServer(tcore::ITcpServer * server) {
    SetLastError(0);
    server->socket_handler = INVALID_SOCKET;
    int len = BUFF_SIZE;

    if (INVALID_SOCKET == (server->socket_handler = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED))) {
        TASSERT(false, "WSASocket error %d", ::GetLastError());
        return false;
    }

    if (SOCKET_ERROR == setsockopt(server->socket_handler, SOL_SOCKET, SO_SNDBUF, (char *)&len, sizeof(int))
        || SOCKET_ERROR == setsockopt(server->socket_handler, SOL_SOCKET, SO_RCVBUF, (char *)&len, sizeof(int))) {
            closesocket(server->socket_handler);
            TASSERT(false, "setsockopt error %d", ::GetLastError());
            return false;
    }

    memset(&server->m_addr, 0, sizeof(server->m_addr));
    server->m_addr.sin_family = AF_INET;
    server->m_addr.sin_port = htons(server->port);
    if(INADDR_NONE == (server->m_addr.sin_addr.s_addr = inet_addr(server->ip))) {
        closesocket(server->socket_handler);
        TASSERT(false, "inet_addr error %d, ip %s", ::GetLastError(), server->ip);
        return false;
    }
    if (SOCKET_ERROR == bind(server->socket_handler, (struct sockaddr*)&(server->m_addr), sizeof(struct sockaddr_in))) {
        closesocket(server->socket_handler);
        TASSERT(false, "bind error %d", ::GetLastError());
        return false;
    }
    if (listen(server->socket_handler, 2048) == SOCKET_ERROR) {
        closesocket(server->socket_handler);
        TASSERT(false, "listen error %d", ::GetLastError());
        return false;
    }

    if (m_hCompletionPortAC != (CreateIoCompletionPort((HANDLE)server->socket_handler, (HANDLE)m_hCompletionPortAC, (u_long)server->socket_handler, 0))) {
        closesocket(server->socket_handler);
        TASSERT(false, "CreateIoCompletionPort error %d", ::GetLastError());
        return false;
    }

    server->DoAccept(0, NULL);

    return true;
}

bool iocper::AddClient(tcore::ITcpSocket * client) {
    SetLastError(0);
    client->socket_handler = INVALID_SOCKET;

    if (INVALID_SOCKET == (client->socket_handler = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) ) {
        TASSERT(false, "WSASocket error %d", ::GetLastError());
        return false;
    }

    memset(&client->m_addr, 0, sizeof(client->m_addr));
    client->m_addr.sin_family = AF_INET;
    if (SOCKET_ERROR == bind(client->socket_handler, (struct sockaddr *)&client->m_addr, sizeof(struct sockaddr_in))) {
        closesocket(client->socket_handler);
        TASSERT(false, "ConnectEx bind error %d", ::GetLastError());
        return false;
    }

    DWORD dwValue = 0;
    if (SOCKET_ERROR == ioctlsocket(client->socket_handler, FIONBIO, &dwValue)) {
        closesocket(client->socket_handler);
        TASSERT(false, "ConnectEx ioctlsocket error %d", ::GetLastError());
        return false;
    }

    HANDLE hCompletionPort = BalancingCompletionPort();
    if (hCompletionPort != CreateIoCompletionPort((HANDLE)client->socket_handler, hCompletionPort, (u_long)client->socket_handler, 0)) {
        closesocket(client->socket_handler);
        TASSERT(false, "CreateIoCompletionPort error %d", ::GetLastError());
        return false;
    }

    struct iocp_event * pEvent = g_poolIocpevent.Create();
    formartIocpevent(pEvent, client, client->socket_handler, SO_CONNECT);
    client->m_addr.sin_family = AF_INET;
    client->m_addr.sin_port = htons(client->port);
    if((client->m_addr.sin_addr.s_addr = inet_addr(client->ip)) == INADDR_NONE) {
        closesocket(client->socket_handler);
        g_poolIocpevent.Recover(pEvent);
        TASSERT(false, "inet_addr error %d", ::GetLastError());
        return false;
    }

    s32 res = g_pFunConnectEx(
        client->socket_handler,
        (struct sockaddr *)&client->m_addr,
        sizeof(struct sockaddr_in),
        NULL,
        0,
        &pEvent->dwBytes,
        (LPOVERLAPPED)pEvent
        );

    s32 err = WSAGetLastError();
    if (res == FALSE && err != WSA_IO_PENDING) {
        closesocket(client->socket_handler);
        g_poolIocpevent.Recover(pEvent);
        TASSERT(false, "ConnectEx error %d", err);
        return false;
    }

    return true;
}

HANDLE iocper::BalancingCompletionPort(HANDLE hCompletionPort) {
    static s32 index = 0;
    if (hCompletionPort == NULL) {
        return m_pCompletionPortAry[index ++ % Configmgr::getInstance()->GetCoreConfig()->sNetThdCount];
    } else {
        return NULL;
    }
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

    ECHO("accept thread %ld", tools::GetCurrentThreadID());
    ITcpServer * server = (ITcpServer *)pEvent->pContext;
    TASSERT(server, "wtf");
    // accept
    server->DoAccept(0, NULL);

    s32 nLen = sizeof(struct sockaddr);
    setsockopt(pEvent->socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, 
        (const char*) &server->socket_handler, sizeof(server->socket_handler));
    if (SOCKET_ERROR == getpeername(pEvent->socket, (struct sockaddr*)&pEvent->remote, &nLen)) {
        server->Error(Kernel::getInstance(), SO_ACCEPT, NULL, "getpeername error");
        shutdown(pEvent->socket, SD_BOTH);
        closesocket(pEvent->socket);
        g_poolIocpevent.Recover(pEvent);
        return;
    }

    HANDLE hCompletionPort = BalancingCompletionPort();
    if (CreateIoCompletionPort((HANDLE)(pEvent->socket), (HANDLE)hCompletionPort, (u_long)(HANDLE)(pEvent->socket), 0) != hCompletionPort) {
        server->Error(Kernel::getInstance(), SO_ACCEPT, NULL, "CreateIoCompletionPort error");
        shutdown(pEvent->socket, SD_BOTH);
        closesocket(pEvent->socket);
        g_poolIocpevent.Recover(pEvent);
        TASSERT(false, "CreateIoCompletionPort error %d", ::GetLastError());
        return;
    }


    ITcpSocket * client = server->MallocConnection(Kernel::getInstance());
    TASSERT(client && client->m_nStatus == SS_UNINITIALIZE, "wtf");
    if (client == NULL || client->m_nStatus != SS_UNINITIALIZE) {
        server->Error(Kernel::getInstance(), SO_ACCEPT, client, "client point null of status not SS_UNINITIALIZE");
        shutdown(pEvent->socket, SD_BOTH);
        closesocket(pEvent->socket);
        g_poolIocpevent.Recover(pEvent);
        return ;
    } 

    client->socket_handler = pEvent->socket;
    TASSERT(pEvent, "wtf");;
    pEvent->pContext = client;
    formartIocpevent(pEvent, client, socket_handler, SO_ACCEPT);
    if (!PostQueuedCompletionStatus(hCompletionPort, 0, socket_handler, (LPWSAOVERLAPPED)pEvent)) {
        // some error must be deal.
        shutdown(socket_handler, SD_BOTH);
        closesocket(socket_handler);
        g_poolIocpevent.Recover(pEvent);
        TASSERT(false, "PostQueuedCompletionStatus error %d", ::GetLastError());
        return;
    }
    client->m_nStatus = SS_ESTABLISHED;
    client->Connected(Kernel::getInstance());
}

s64 iocper::DonetIO(s64 overtime) {
    s64 lTick = tools::GetTimeMillisecond();

    CompletedAccept();

    for (s32 i=0; i<m_nWorkerCount; i++) {
        m_pIocpworkerAry[i].DealEvent(overtime/4);
    }
    return tools::GetTimeMillisecond() - lTick;
}
