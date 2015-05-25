#include "iocpworker.h"
#include "Kernel.h"
#include "Header.h"
#include "CPipe.h"

iocpworker::iocpworker() : m_hCompletionPort(NULL) {

}

bool iocpworker::Initialize() {
    if (NULL == (m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0))) {
        TASSERT(false, "CreateIoCompletionPort error %d", ::GetLastError());
        return false;
    }
    m_nStatus = THREAD_STOPED;

    return true;
}


bool iocpworker::AddPIPE(CPipe * pCPipe) {
    if (CreateIoCompletionPort((HANDLE)(pCPipe->m_lSocketHandler), (HANDLE)m_hCompletionPort, (u_long)(HANDLE)(pCPipe->m_lSocketHandler), 0) != m_hCompletionPort) {
        closesocket(pCPipe->m_lSocketHandler);
        TASSERT(false, "CreateIoCompletionPort error %d", ::GetLastError());
        return false;
    }
    pCPipe->m_hCompletionPort = m_hCompletionPort;
    return true;
}

s64 iocpworker::Processing(s64 overtime) {
    s64 lTick = tools::GetTimeMillisecond();

   while (true) {
        ioevent event;
        if (m_oEventQueue.Read(event)) {
            CPipe * pCPipe = event.pCPipe;
            TASSERT(pCPipe, "wtf");
            switch (event.type) {
            case IO_EVENT_TYPE_RECV:
                {
                    s32 nUse = 0;
                    pCPipe->m_oRecvStream.LockRead();
                    if ( 0 != pCPipe->m_oRecvStream.size() ) {
                        nUse = pCPipe->m_pHost->OnRecv(Kernel::getInstance(), pCPipe->m_oRecvStream.buff(), pCPipe->m_oRecvStream.size());
                        if (nUse > 0) {
                            pCPipe->m_oRecvStream.out(nUse);
                        }
                    }
                    pCPipe->m_oRecvStream.FreeRead();

                    break;
                }
            case IO_EVENT_TYPE_BREAK:
                {             
                    TASSERT(SS_UNINITIALIZE != pCPipe->m_nStatus, "check status modify");
                    pCPipe->m_nStatus = SS_UNINITIALIZE;
                    pCPipe->m_pHost->m_pPipe = NULL;
                    pCPipe->m_pHost->OnDisconnect(Kernel::getInstance());
                    g_oCPipePool.Recover(pCPipe); // delete pCPipe;
                    break;
                }
            case IO_EVENT_TYPE_CONNECTED:
                {
                    pCPipe->m_nStatus = SS_ESTABLISHED;
                    formartIocpevent(pCPipe->m_pRecvEvent, pCPipe, pCPipe->m_lSocketHandler, SO_ACCEPT);
                    bool res = PostQueuedCompletionStatus(pCPipe->m_hCompletionPort, 0, pCPipe->m_lSocketHandler, (LPWSAOVERLAPPED)pCPipe->m_pRecvEvent);
                    TASSERT(res, "PostQueuedCompletionStatus error %d", ::GetLastError());
                    pCPipe->m_pHost->OnConnected(Kernel::getInstance());
                    break;
                }
            case IO_EVENT_TYPE_CONNECTFAILD:
                {
                    pCPipe->m_nStatus = SS_UNINITIALIZE;
                    pCPipe->m_pHost->OnConnectFailed(Kernel::getInstance());
                    g_oCPipePool.Recover(pCPipe); // delete pCPipe;
                    break;
                }
            }
        } else if (m_oEventQueue.IsEmpty()) {
            return tools::GetTimeMillisecond() - lTick;
        }

        s64 lDoneTick = tools::GetTimeMillisecond() - lTick;
        if (lDoneTick >= overtime) {
            return overtime - lDoneTick;
        }
    }

    return 0;
}

bool iocpworker::CheckSocketClient(const s64 socket, CPipe * pCPipe) {
    SOCKET_CLIENT_RELATION::iterator itor = m_mapSocketClient.find(socket);
    if (itor != m_mapSocketClient.end()) {
        if (itor->second == pCPipe) {
            return true;
        }
    }

    return false;
}

void iocpworker::RelateSocketClient(const s64 socket, CPipe * pClient) {
    SOCKET_CLIENT_RELATION::iterator itor = m_mapSocketClient.find(socket);
    TASSERT(itor == m_mapSocketClient.end(), "wtf");
    if (itor == m_mapSocketClient.end()) {
        m_mapSocketClient.insert(make_pair(socket, pClient));
    }
}

void iocpworker::SendDisconnectEvent(const s64 socket, CPipe * pCPipe) {
    SOCKET_CLIENT_RELATION::iterator itor = m_mapSocketClient.find(socket);
    if (itor != m_mapSocketClient.end()) {
        if (itor->second == pCPipe) {
            ioevent event; // m_oEventPool.Create();
            event.type = IO_EVENT_TYPE_BREAK;
            event.pCPipe = pCPipe;
            shutdown(pCPipe->m_lSocketHandler, SD_BOTH);

            linger InternalLinger;
            InternalLinger.l_onoff = 1;
            InternalLinger.l_linger = 0;
            setsockopt(pCPipe->m_lSocketHandler, SOL_SOCKET, SO_LINGER, (const char*)&InternalLinger, sizeof(linger));
            closesocket(pCPipe->m_lSocketHandler);
            pCPipe->m_lSocketHandler = INVALID_SOCKET;
            m_oEventQueue.Add(event);
            m_mapSocketClient.erase(itor);
        }
    }
}

void iocpworker::Terminate() {
    if (THREAD_WORKING == m_nStatus) {
        m_nStatus = THREAD_STOPPING;
        while (m_nStatus != THREAD_STOPED) {
            CSLEEP(1);
        }
    }
}

void iocpworker::Run() {
    m_nStatus = THREAD_WORKING;
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

        pEvent->code = WSAGetLastError();
        if (!nSucceed ) {
            if (WAIT_TIMEOUT == pEvent->code) {
                CSLEEP(1);
                continue;
            }
        }

        CPipe * pCPipe = (CPipe *)pEvent->pContext;
        switch (pEvent->opt) {
        case SO_CONNECT:
            {
                ioevent event;// m_oEventPool.Create();
                event.pCPipe = pCPipe;
                (0 == pEvent->code)?(event.type = IO_EVENT_TYPE_CONNECTED):(event.type = IO_EVENT_TYPE_CONNECTFAILD);
                m_oEventQueue.Add(event);
                break;
            }
        case SO_ACCEPT:
            {
                TASSERT(pCPipe->m_nStatus != SS_UNINITIALIZE, "wtf");
                RelateSocketClient(pCPipe->m_lSocketHandler, pCPipe);
                if (SS_ESTABLISHED == pCPipe->m_nStatus) {
                     pCPipe->DoRecv();
                } else {
                    if (pCPipe->m_bNeedPostOPT) {
                        SendDisconnectEvent(socket_handler, pCPipe);
                    }
                }

                break;
            }
        case SO_TCPRECV:
            {
                if ( CheckSocketClient(socket_handler, pCPipe) ) {
                    TASSERT(pCPipe && !(nIOBytes > 0 && ERROR_SUCCESS != pEvent->code), "wtf");
                    TASSERT(pEvent == pCPipe->m_pRecvEvent, "wtf");
                    if (nIOBytes > 0 && ERROR_SUCCESS == pEvent->code) {
                        //data recved, write into recvstream
                        pCPipe->In(pEvent->wbuf.buf, nIOBytes);

                        //post wsarecv opt
                        if (!pCPipe->DoRecv()) {
                            SendDisconnectEvent(socket_handler, pCPipe);
                        } else {
                            ioevent event; //m_oEventPool.Create(); // NEW ioevent; //m_oEventPool.Create();
                            event.type = IO_EVENT_TYPE_RECV;
                            event.pCPipe = pCPipe;
                            m_oEventQueue.Add(event);
                        }
                    } else if (nIOBytes <= 0 || ERROR_SUCCESS != pEvent->code) {
                        SendDisconnectEvent(socket_handler, pCPipe);
                    }
                    break;
                }
                break;
            }
        case SO_TCPSEND:
            {
                if ( CheckSocketClient(socket_handler, pCPipe) ) {
                    TASSERT(pCPipe && !(nIOBytes > 0 && ERROR_SUCCESS != pEvent->code), "wtf");
                    TASSERT(pEvent == pCPipe->m_pSendEvent, "wtf");
                    if (ERROR_SUCCESS == pEvent->code) {
                        //data recved, write into recvstream
                        if (!pCPipe->Out(nIOBytes)) {
                            SendDisconnectEvent(socket_handler, pCPipe);
                        }
                    }
                    break;
                }
                break;
            }

        }
    }
}

