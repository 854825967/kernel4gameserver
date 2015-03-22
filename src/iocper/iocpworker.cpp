#include "iocpworker.h"
#include "Kernel.h"

s64 iocpworker::DealEvent(s64 overtime) {
    s64 lTick = tools::GetTimeMillisecond();

    while (true) {
        struct iocp_event * pEvent = NULL;
        if (m_queueEvent.Read(pEvent)) {
            TASSERT(pEvent, "wtf");

            switch (pEvent->opt) {
            case SO_CONNECT:
                {
                    ITcpSocket * client = (ITcpSocket *) pEvent->pContext;
                    if (ERROR_SUCCESS == pEvent->code) {
                        client->socket_handler = pEvent->socket;
                        TASSERT(pEvent, "wtf");;
                        pEvent->pContext = client;
                        formartIocpevent(pEvent, client, client->socket_handler, SO_ACCEPT);
                        if (!PostQueuedCompletionStatus(m_hCompletionPort, 0, client->socket_handler, (LPWSAOVERLAPPED)pEvent)) {
                            // some error must be deal.
                            shutdown(client->socket_handler, SD_BOTH);
                            closesocket(client->socket_handler);
                            g_poolIocpevent.Recover(pEvent);
                            TASSERT(false, "PostQueuedCompletionStatus error %d", ::GetLastError());
                            client->Error(Kernel::getInstance(), SO_CONNECT, NULL, "connect error");
                            break;
                        }
                        client->m_nStatus = SS_ESTABLISHED;
                        client->Connected(Kernel::getInstance());
                    } else {
                        client->Error(Kernel::getInstance(), SO_CONNECT, NULL, "connect error");
                    }
                    break;
                }
            case SO_TCPRECV:
                {
                    ITcpSocket * client = (ITcpSocket *) pEvent->pContext;
                    if (pEvent->code != 0 || pEvent->dwBytes == 0) {
                        TASSERT(SS_UNINITIALIZE != client->m_nStatus, "check status modify");
                        if (SS_UNINITIALIZE != client->m_nStatus) {
                            client->m_nStatus = SS_UNINITIALIZE;
                            client->Disconnect(Kernel::getInstance());
                        }
                    } else {
                        s32 nUse = 0;
                        while (0 != client->m_recvStream.size() &&
                            0 != (nUse = client->Recv(Kernel::getInstance(), client->m_recvStream.buff(), client->m_recvStream.size())) ) {
                                client->m_recvStream.out(nUse);
                        }
                    }
                    g_poolIocpevent.Recover(pEvent);
                    s32 i = 100;
                    break;
                }
            }
        } else if (m_queueEvent.IsEmpty()) {
            return tools::GetTimeMillisecond() - lTick;
        }

        s64 lDoneTick = tools::GetTimeMillisecond() - lTick;
        if (lDoneTick >= overtime) {
            return overtime - lDoneTick;
        }
    }
}

void iocpworker::RelateSocketClient(const s64 socket, ISocket * pClient) {
    SOCKET_CLIENT_RELATION::iterator itor = m_mapSocketClient.find(socket);
    TASSERT(itor == m_mapSocketClient.end(), "wtf");
    if (itor == m_mapSocketClient.end()) {
        m_mapSocketClient.insert(make_pair(socket, pClient));
    }
}

bool iocpworker::CheckSocketClient(const s64 socket) {
    SOCKET_CLIENT_RELATION::iterator itor = m_mapSocketClient.find(socket);
    return itor != m_mapSocketClient.end();
}

void iocpworker::SendDisconnectEvent(struct iocp_event * & pEvent) {
    SOCKET_CLIENT_RELATION::iterator itor = m_mapSocketClient.find(pEvent->socket);
    if (itor != m_mapSocketClient.end() && itor->second == pEvent->pContext) {
        closesocket(pEvent->socket);
        pEvent->dwBytes = 0;
        pEvent->code = -1;
        pEvent->opt = SO_TCPRECV;
        m_queueEvent.Add(pEvent);
        m_mapSocketClient.erase(itor);
    } else {
        ECHO("double close %d", pEvent->socket);
        g_poolIocpevent.Recover(pEvent);
    }

}

void iocpworker::Run() {
    ECHO("iocpworker run thread %ld", tools::GetCurrentThreadID());
    while (true) {
        if (THREAD_STOPPING == m_nStatus) {
            m_nStatus = THREAD_STOPED;
            return;
        }

        DWORD nIOBytes = 0;
        s64  socket_handler = INVALID_SOCKET;
        struct iocp_event * pEvent = NULL;
        SetLastError(0);
        BOOL nSucceed = GetQueuedCompletionStatus(m_hCompletionPort, &nIOBytes, (PULONG_PTR)&socket_handler, (LPOVERLAPPED *)&pEvent, 10);

        if(NULL == pEvent) {
            CSLEEP(1);
            continue;
        }

        pEvent->code = GetLastError();
        if (!nSucceed ) {
            if (WAIT_TIMEOUT == pEvent->code) {
                CSLEEP(1);
                continue;
            }
        }
        
        switch (pEvent->opt) {
        case SO_CONNECT:
            {
                m_queueEvent.Add(pEvent);
                break;
            }
        case SO_ACCEPT:
            {
                ITcpSocket * pClient = (ITcpSocket *)pEvent->pContext;
                TASSERT(pClient, "wtf");
                RelateSocketClient(pClient->socket_handler, pClient);
                if ( !pClient->DoRecv(pEvent, m_hCompletionPort) ) {
                    SendDisconnectEvent(pEvent);
                    break;
                }

                struct iocp_event * pSEvent = g_poolIocpevent.Create();
                TASSERT(pSEvent, "wtf");
                if (!pClient->DoSend(pSEvent, m_hCompletionPort)) {
                    SendDisconnectEvent(pSEvent);
                }
                break;
            }
        case SO_TCPRECV:
            {
                if (!CheckSocketClient(pEvent->socket)) {
                    g_poolIocpevent.Recover(pEvent);
                    break;
                }
                pEvent->dwBytes = nIOBytes;
                ITcpSocket * client = (ITcpSocket *)pEvent->pContext;
                TASSERT(client && !(pEvent->dwBytes > 0 && ERROR_SUCCESS != pEvent->code), "wtf");
                if (pEvent->dwBytes > 0 && ERROR_SUCCESS == pEvent->code) {
                    //data recved, write into recvstream
                    client->m_recvStream.in(pEvent->wbuf.buf, nIOBytes);
                    m_queueEvent.Add(pEvent);

                    //post wsarecv opt
                    pEvent = g_poolIocpevent.Create();
                    if (!client->DoRecv(pEvent, m_hCompletionPort)) {
                        SendDisconnectEvent(pEvent);
                    }
                } else if (pEvent->dwBytes <= 0 || ERROR_SUCCESS != pEvent->code) {
                    SendDisconnectEvent(pEvent);
                }
                break;
            }
        case SO_TCPSEND:
            {
                if (!CheckSocketClient(pEvent->socket)) {
                    g_poolIocpevent.Recover(pEvent);
                    break;
                }
                pEvent->dwBytes = nIOBytes;
                ITcpSocket * client = (ITcpSocket *)pEvent->pContext;
                TASSERT(client && !(nIOBytes>0 && ERROR_SUCCESS != pEvent->code), "wtf")

                if (ERROR_SUCCESS == pEvent->code) {
                    if (nIOBytes > 0) {
                        client->m_sendStream.out(nIOBytes);
                    }

                    if (!client->DoSend(pEvent, m_hCompletionPort)) {
                        SendDisconnectEvent(pEvent);
                    }
                } else {
                    SendDisconnectEvent(pEvent);
                }

                break;
            }

        }
    }
}

