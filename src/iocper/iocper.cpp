#include "iocper.h"
#include "Tools.h"
#include "Kernel.h"
#include "configmgr/Configmgr.h"
#include "CLock.h"
using namespace tcore;
using namespace tlib;

LPFN_ACCEPTEX iocper::s_pFunAcceptEx = NULL;
LPFN_CONNECTEX iocper::s_pFunConnectEx = NULL;

inline LPFN_ACCEPTEX GetAcceptExFun() {
    GUID GuidAcceptEx = WSAID_ACCEPTEX;
    DWORD dwBytes = 0;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    LPFN_ACCEPTEX pAcceptFun = NULL;
    WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx,
        sizeof(GuidAcceptEx), &pAcceptFun, sizeof(pAcceptFun),
        &dwBytes, NULL, NULL);

    if (NULL == pAcceptFun) {
        s32 nError = WSAGetLastError();
        TASSERT(false, "Get AcceptEx fun error, error code : %d", nError);
    }

    return pAcceptFun;
}

inline LPFN_CONNECTEX GetConnectExFun() {
    GUID GuidConnectEx = WSAID_CONNECTEX;
    DWORD dwBytes = 0;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    LPFN_CONNECTEX pConnectFun = NULL;
    WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx,
        sizeof(GuidConnectEx), &pConnectFun, sizeof(pConnectFun),
        &dwBytes, NULL, NULL);

    if (NULL == pConnectFun) {
        TASSERT(false, "Get ConnectEx fun error, error code : %d", WSAGetLastError());
    }

    return pConnectFun;
}

inline void formartIocpevent(struct iocp_event * & pEvent, ISocket * p,
                             const s64 socket, const s8 event, const s8 iotype) {
    memset(pEvent, 0, sizeof(struct iocp_event));
    pEvent->wbuf.buf = pEvent->buff;
    pEvent->wbuf.len = sizeof(pEvent->buff);
    pEvent->socket = socket;
    pEvent->p = p;
    pEvent->event = event;
    pEvent->iotype = iotype;
}

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
    s32 count = Configmgr::getInstance()->GetCoreConfig()->sNetThdCount;

    SetLastError(0);
    if (NULL == (s_pFunAcceptEx = GetAcceptExFun())) {
        TASSERT(false, "GetAcceptExFun error %d", ::GetLastError());
        return false;
    }

    if (NULL == (s_pFunConnectEx = GetConnectExFun())) {
        TASSERT(false, "GetConnectExFun error %d", ::GetLastError());
        return false;
    }
    if (NULL == (m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0))) {
        TASSERT(false, "CreateIoCompletionPort error %d", ::GetLastError());
        return false;
    }

    m_bShutdown = false;

    Start(count);

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
    if (listen(server->socket_handler, 256) == SOCKET_ERROR) {
        closesocket(server->socket_handler);
        TASSERT(false, "listen error %d", ::GetLastError());
        return false;
    }

    if (m_hCompletionPort != (CreateIoCompletionPort((HANDLE)server->socket_handler, (HANDLE)m_hCompletionPort, (u_long)server->socket_handler, 0))) {
        closesocket(server->socket_handler);
        TASSERT(false, "CreateIoCompletionPort error %d", ::GetLastError());
        return false;
    }

    struct iocp_event * pEvent = m_poolIocpevent.Create();
    if (NULL == pEvent) {
        closesocket(server->socket_handler);
        TASSERT(false, "create iocp_event error, check pool template");
        return false;
    }
    memset(pEvent, 0, sizeof(*pEvent));

    pEvent->p = server;
    pEvent->event = SO_ACCEPT;

    if (INVALID_SOCKET == (pEvent->socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))) {
        closesocket(server->socket_handler);
        m_poolIocpevent.Recover(pEvent);
        TASSERT(false, "WSASocket error %d", ::GetLastError());
        return false;
    }

    s32 res = s_pFunAcceptEx(
        server->socket_handler,
        pEvent->socket,
        pEvent->buff,
        0,
        sizeof(struct sockaddr_in) + 16,
        sizeof(struct sockaddr_in) + 16,
        &pEvent->dwBytes,
        (LPOVERLAPPED)pEvent
        );

    s32 err = WSAGetLastError();
    if (res == FALSE && err != WSA_IO_PENDING) {
        closesocket(server->socket_handler);
        m_poolIocpevent.Recover(pEvent);
        TASSERT(false, "AcceptEx error %d", err);
        return false;
    }

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

    if (m_hCompletionPort != CreateIoCompletionPort((HANDLE)client->socket_handler, m_hCompletionPort, (u_long)client->socket_handler, 0)) {
        closesocket(client->socket_handler);
        TASSERT(false, "CreateIoCompletionPort error %d", ::GetLastError());
        return false;
    }

    struct iocp_event * pEvent = m_poolIocpevent.Create();
    memset(pEvent, 0, sizeof(*pEvent));
    pEvent->p = client;
    pEvent->event = SO_CONNECT;
    client->m_addr.sin_family = AF_INET;
    client->m_addr.sin_port = htons(client->port);
    if((client->m_addr.sin_addr.s_addr = inet_addr(client->ip)) == INADDR_NONE) {
        closesocket(client->socket_handler);
        m_poolIocpevent.Recover(pEvent);
        TASSERT(false, "inet_addr error %d", ::GetLastError());
        return false;
    }


    s32 res = s_pFunConnectEx(
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
        m_poolIocpevent.Recover(pEvent);
        TASSERT(false, "ConnectEx error %d", err);
        return false;
    }

    return true;
}

void iocper::Run() {
    {
        CLock lock(&m_threadLock);
        m_nThreadCount++;
    }

    DWORD nIOBytes;
    BOOL nSucceed;
    s64 s;
    struct iocp_event * pEvent;
    SetLastError(0);
    while (true) {
        if (m_bShutdown) {
            CLock lock(&m_threadLock);
            m_nThreadCount--;
            return;
        }

        nIOBytes = 0;
        s = INVALID_SOCKET;
        SetLastError(0);
        pEvent = NULL;
        nSucceed = GetQueuedCompletionStatus(m_hCompletionPort, &nIOBytes, (PULONG_PTR)&s, (LPOVERLAPPED *)&pEvent, 10);

        if(NULL == pEvent) {
            CSLEEP(1);
            continue;
        }

        pEvent->nerron = GetLastError();
        if (!nSucceed ) {
            if (WAIT_TIMEOUT == pEvent->nerron) {
                CSLEEP(1);
                continue;
            }
        }

        switch (pEvent->event) {
        case SO_ACCEPT:
            {
                ISocket * server = pEvent->p;
                m_queueIocpevent.Add(pEvent);                    
                // accept
                pEvent = m_poolIocpevent.Create();
                if (NULL == pEvent) {
                    TASSERT(false, "where is iocp_event");
                    break;
                }
                memset(pEvent, 0, sizeof(*pEvent));
                pEvent->p = server;
                pEvent->event = SO_ACCEPT;
                CLock lock(&server->m_lockAccept);
                if (INVALID_SOCKET == (pEvent->socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))) {
                    TASSERT(false, "WSASocket error %d", ::GetLastError());
                    m_poolIocpevent.Recover(pEvent);
                    break;
                }

                s32 res = s_pFunAcceptEx(server->socket_handler, pEvent->socket, pEvent->buff, 0,
                    sizeof(struct sockaddr_in) + 16, sizeof(struct sockaddr_in) + 16, &pEvent->dwBytes, (LPOVERLAPPED)pEvent);

                s32 err = WSAGetLastError();
                if (res == FALSE && err != WSA_IO_PENDING) {
                    closesocket(pEvent->socket);
                    m_poolIocpevent.Recover(pEvent);
//                    TASSERT(false, "AcceptEx error %d", ::GetLastError());
                    break;
                }
                break;
            }
        case SO_TCPIO:
            {
                ITcpSocket * client = (ITcpSocket *)pEvent->p;
                pEvent->ioBytes = nIOBytes;
                switch(pEvent->iotype) {
                case IO_TYPE_RECV:
                    {
                        if (pEvent->ioBytes == 0 || ERROR_SUCCESS != pEvent->nerron) {
                            shutdown(client->socket_handler, SD_BOTH);
                            closesocket(client->socket_handler);
                            pEvent->ioBytes = 0;
                            m_queueIocpevent.Add(pEvent);
                        } else {
                            client->m_recvStream.in(pEvent->wbuf.buf, pEvent->ioBytes);
                            m_queueIocpevent.Add(pEvent);
                            pEvent = m_poolIocpevent.Create();
                            TASSERT(pEvent, "where is iocp event");
                            SetLastError(0);
                            formartIocpevent(pEvent, client, client->socket_handler, SO_TCPIO, IO_TYPE_RECV);

                            if (SOCKET_ERROR == WSARecv(pEvent->socket, &pEvent->wbuf, 1, 
                                &pEvent->dwBytes, &pEvent->dwFlags, (LPWSAOVERLAPPED)pEvent, NULL)) {
                                    pEvent->nerron = GetLastError();
                                    if (ERROR_IO_PENDING != pEvent->nerron) {
                                        shutdown(client->socket_handler, SD_BOTH);
                                        closesocket(client->socket_handler);
                                        pEvent->ioBytes = 0;
                                        m_queueIocpevent.Add(pEvent);
                                        break;
                                    }
                            }
                        }
                        break;
                    }
                case IO_TYPE_SEND:
                    {
                        if (0 != pEvent->ioBytes) {
                            client->m_sendStream.out(pEvent->ioBytes);
                        }

                        if (0 == client->m_sendStream.size()) {
                            if (client->m_nStatus == SS_WAITCLOSE) {
                                shutdown(client->socket_handler, SD_BOTH);
                                closesocket(client->socket_handler);
                                m_poolIocpevent.Recover(pEvent);
                            } else {
                                if (!PostQueuedCompletionStatus(m_hCompletionPort, 0, client->socket_handler, (LPWSAOVERLAPPED)pEvent)) {
                                    // some error must be deal.
                                    shutdown(client->socket_handler, SD_BOTH);
                                    closesocket(client->socket_handler);
                                    m_poolIocpevent.Recover(pEvent);
                                    TASSERT(false, "PostQueuedCompletionStatus error %d", ::GetLastError());
                                }
                            }
                        } else {
                            SetLastError(0);
                            pEvent->wbuf.buf = (char *)client->m_sendStream.buff();
                            pEvent->wbuf.len = client->m_sendStream.size();
                            s32 res = WSASend(client->socket_handler, &pEvent->wbuf, 1, NULL, 0, (LPWSAOVERLAPPED)pEvent, NULL);
                            if (SOCKET_ERROR == res) {
                                pEvent->nerron = WSAGetLastError();
                                if (pEvent->nerron != WSA_IO_PENDING) {
                                    break;
                                } else {
                                    shutdown(client->socket_handler, SD_BOTH);
                                    closesocket(client->socket_handler);
                                    m_poolIocpevent.Recover(pEvent);
                                    break;
                                }
                            }
                        }
                        break;
                    }
                default:
                    TASSERT(false, "what's this");
                    break;
                }

            }
        }
    }
}

s64 iocper::DonetIO(s64 overtime) {
    s64 lTick = tools::GetTimeMillisecond();

    struct iocp_event * pEvent;
    while (true) {
        if (m_queueIocpevent.Read(pEvent)) {
            switch (pEvent->event) {
            case SO_ACCEPT:
                {
                    ITcpServer * server = (ITcpServer *) pEvent->p;
                    TASSERT(server, "where is server");
                    if (ERROR_SUCCESS == pEvent->nerron) {
                        s32 nLen = sizeof(struct sockaddr);
                        setsockopt(pEvent->socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, 
                            (const char*) &server->socket_handler, sizeof(server->socket_handler));
                        if (SOCKET_ERROR == getpeername(pEvent->socket, (struct sockaddr*)&pEvent->remote, &nLen)) {
                            server->Error(Kernel::getInstance(), SO_ACCEPT, ::GetLastError());
//                            TASSERT(false, "getpeername error %d", ::GetLastError());
                            shutdown(pEvent->socket, SD_BOTH);
                            closesocket(pEvent->socket);
                            m_poolIocpevent.Recover(pEvent);
                            break;
                        }

                        HANDLE hCompletionPort = CreateIoCompletionPort((HANDLE)(pEvent->socket), (HANDLE)m_hCompletionPort, (u_long)(HANDLE)(pEvent->socket), 0);

                        ECHO("CreateIoCompletionPort event address %ld, socket %ld", pEvent, pEvent->socket);
                        if (m_hCompletionPort != hCompletionPort) {
                            closesocket(pEvent->socket);
                            m_poolIocpevent.Recover(pEvent);
                            s32 nError = ::GetLastError();
                            server->Error(Kernel::getInstance(), SO_ACCEPT, nError);
                            TASSERT(false, "CreateIoCompletionPort error %d", nError);
                            break;
                        }

                        ITcpSocket * client = server->MallocConnection(Kernel::getInstance());
                        if (!client) {
                            TASSERT(client, "where is client");
                            closesocket(pEvent->socket);
                            m_poolIocpevent.Recover(pEvent);
                            break;
                        }
                        client->socket_handler = pEvent->socket;
                        SetLastError(0);

                        formartIocpevent(pEvent, client, client->socket_handler, SO_TCPIO, IO_TYPE_RECV);
                        if (SOCKET_ERROR == WSARecv(pEvent->socket, &pEvent->wbuf, 1, &pEvent->dwBytes, &pEvent->dwFlags, (LPWSAOVERLAPPED)pEvent, NULL)) {
                            s32 error = GetLastError();
                            if (ERROR_IO_PENDING != error) {
                                client->Error(Kernel::getInstance(), SO_TCPIO, error);
                                shutdown(client->socket_handler, SD_BOTH);
                                closesocket(client->socket_handler);
                                m_poolIocpevent.Recover(pEvent);
                                //TASSERT(false, "WSARecv error %d", error);
                                break;
                            }
                        }

                        struct iocp_event * pNewEvent = m_poolIocpevent.Create();
                        if (!pNewEvent) {
                            client->Error(Kernel::getInstance(), SO_TCPIO, -1);
                            shutdown(client->socket_handler, SD_BOTH);
                            closesocket(client->socket_handler);
                            TASSERT(pNewEvent, "where is iocp event");
                            break;
                        }

                        formartIocpevent(pNewEvent, client, client->socket_handler, SO_TCPIO, IO_TYPE_SEND);
                        if (!PostQueuedCompletionStatus(m_hCompletionPort, 0, client->socket_handler, (LPWSAOVERLAPPED)pNewEvent)) {
                            shutdown(client->socket_handler, SD_BOTH);
                            closesocket(client->socket_handler);
                            m_poolIocpevent.Recover(pNewEvent);
                            TASSERT(false, "PostQueuedCompletionStatus error %d", ::GetLastError());
                        }

                        client->Connected(Kernel::getInstance());
                        client->m_nStatus = SS_ESTABLISHED;
                    }

                    break;
                }
//             case SO_CONNECT:
//                 {
//                     ITcpSocket * client = (ITcpSocket *) p->user_ptr;
//                     client->DoConnect(p->flags, this);
//                     g_EpollerDataPool.Recover(p);
//                     break;
//                 }
            case SO_TCPIO:
                {
                    ITcpSocket * client = (ITcpSocket *) pEvent->p;
                    TASSERT(pEvent->iotype == IO_TYPE_RECV, "check ur io logic");

                    if (pEvent->nerron != 0) {
                        //client->Error(Kernel::getInstance(), pEvent->event, pEvent->nerron);
                        client->Disconnect(Kernel::getInstance());
                    } else {
                        if (pEvent->ioBytes == 0) {
                            client->Disconnect(Kernel::getInstance());
                        } else {
                            s32 nUse = client->Recv(Kernel::getInstance(), (void *)client->m_recvStream.buff(), client->m_recvStream.size());
                            client->m_recvStream.out(nUse);
                        }
                    }
                    m_poolIocpevent.Recover(pEvent);
                    break;
                }
            }
        } else if (m_queueIocpevent.IsEmpty()) {
            return tools::GetTimeMillisecond() - lTick;
        }

        s64 lDoneTick = tools::GetTimeMillisecond() - lTick;
        if (lDoneTick >= overtime) {
            return overtime - lDoneTick;
        }
    }
}
