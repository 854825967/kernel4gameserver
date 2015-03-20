#include "iocpworker.h"
#include "Kernel.h"

s64 iocpworker::DealEvent(s64 overtime) {
    s64 lTick = tools::GetTimeMillisecond();

    while (true) {
        struct iocp_event * pEvent = NULL;
        if (m_queueEvent.Read(pEvent)) {
            TASSERT(pEvent, "wtf");

            switch (pEvent->opt) {
            case SO_TCPRECV:
                {
                    ITcpSocket * client = (ITcpSocket *) pEvent->pContext;
                    if (pEvent->code != 0) {
                        TASSERT(SS_UNINITIALIZE != client->m_nStatus, "check status modify");
                        if (SS_UNINITIALIZE != client->m_nStatus) {
                            client->m_nStatus = SS_UNINITIALIZE;
                            client->Disconnect(Kernel::getInstance());
                        }
                    } else {
                        if (pEvent->dwBytes == 0) {
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
                    }
                    g_poolIocpevent.Recover(pEvent);
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
        case SO_ACCEPT:
            {

                ECHO("work thread %ld", tools::GetCurrentThreadID());
                ITcpSocket * pClient = (ITcpSocket *)pEvent->pContext;
                if ( !pClient->DoRecv(pEvent, m_hCompletionPort) ) {
                    pEvent->dwBytes = 0;
                    m_queueEvent.Add(pEvent);
                    break;
                }

                struct iocp_event * pSEvent = g_poolIocpevent.Create();
                TASSERT(pSEvent, "wtf");
                pClient->DoSend(pSEvent, m_hCompletionPort);
                break;
            }
        case SO_TCPRECV:
            {

                ECHO("work thread %ld", tools::GetCurrentThreadID());
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
                        if (client->m_bSendPending) {
                            g_poolIocpevent.Recover(pEvent);
                        } else {
                            pEvent->dwBytes = 0;
                            pEvent->code = -1;
                            m_queueEvent.Add(pEvent);
                        }
                    }
                } else if (pEvent->dwBytes <= 0 || ERROR_SUCCESS != pEvent->code) {
                    shutdown(client->socket_handler, SD_BOTH);
                    closesocket(client->socket_handler);
                    if (client->m_bSendPending) {
                        g_poolIocpevent.Recover(pEvent);
                    } else {
                        pEvent->dwBytes = 0;
                        pEvent->code = -1;
                        m_queueEvent.Add(pEvent);
                    }
                }
                break;
            }
        case SO_TCPSEND:
            {
                pEvent->dwBytes = nIOBytes;
                ITcpSocket * client = (ITcpSocket *)pEvent->pContext;
                TASSERT(client && !(nIOBytes>0 && ERROR_SUCCESS != pEvent->code), "wtf")
                if (ERROR_SUCCESS == pEvent->code) {
                    if (nIOBytes > 0) {
                        client->m_sendStream.out(nIOBytes);
                    }

                    if (!client->DoSend(pEvent, m_hCompletionPort)) {
                        if (client->m_bRecvPending) {
                            g_poolIocpevent.Recover(pEvent);
                        } else {
                            pEvent->opt = SO_TCPRECV;
                            pEvent->dwBytes = 0;
                            pEvent->code = -1;
                            m_queueEvent.Add(pEvent);
                        }
                    }
                } else {
                    shutdown(client->socket_handler, SD_BOTH);
                    closesocket(client->socket_handler);
                    if (client->m_bRecvPending) {
                        g_poolIocpevent.Recover(pEvent);
                    } else {
                        pEvent->opt = SO_TCPRECV;
                        pEvent->dwBytes = 0;
                        pEvent->code = -1;
                        m_queueEvent.Add(pEvent);
                    }
                }

                break;
            }

        }
    }
}

