#include "iocphead.h"
#include "iocper.h"
#include "Kernel.h"

namespace tcore {

    void ITcpServer::DoAccept(s32 flags, void * pContext) {
        struct iocp_event * pEvent = g_poolIocpevent.Create();
        if (NULL == pEvent) {
            shutdown(socket_handler, SD_BOTH);
            closesocket(socket_handler);
            TASSERT(false, "create iocp_event error, check pool template");
            return;
        }
        formartIocpevent(pEvent, this, INVALID_SOCKET, SO_ACCEPT);
        if (INVALID_SOCKET == (pEvent->socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))) {
            shutdown(socket_handler, SD_BOTH);
            closesocket(socket_handler);
            g_poolIocpevent.Recover(pEvent);
            TASSERT(false, "WSASocket error %d", ::GetLastError());
            return;
        }

        s32 res = g_pFunAcceptEx(
            socket_handler,
            pEvent->socket,
            pEvent->pBuff,
            0,
            sizeof(struct sockaddr_in) + 16,
            sizeof(struct sockaddr_in) + 16,
            &pEvent->dwBytes,
            (LPOVERLAPPED)pEvent
            );

        s32 err = WSAGetLastError();
        if (res == FALSE && err != WSA_IO_PENDING) {
            shutdown(socket_handler, SD_BOTH);
            closesocket(socket_handler);
            g_poolIocpevent.Recover(pEvent);
            TASSERT(false, "AcceptEx error %d", err);
            return;
        }
    }


    void ITcpSocket::DoConnect(s32 flags, void * pContext) {

    }

    bool ITcpSocket::DoRecv(iocp_event * pEvent, HANDLE hCompletionPort) {
        formartIocpevent(pEvent, this, socket_handler, SO_TCPRECV);
        if (m_nStatus == SS_ESTABLISHED) {
            TASSERT(pEvent && hCompletionPort, "r u joking me");
            SetLastError(0);
            if (SOCKET_ERROR == WSARecv(pEvent->socket, &pEvent->wbuf, 1, &pEvent->dwBytes, &pEvent->dwFlags, (LPWSAOVERLAPPED)pEvent, NULL)) {
                pEvent->code = GetLastError();
                if (ERROR_IO_PENDING != pEvent->code) {
                    return false;
                }
            }
            return true;
        } else {
            return false;
        }
        return true;
    }

    bool ITcpSocket::DoSend(iocp_event * pEvent, HANDLE hCompletionPort) {
        TASSERT(pEvent && hCompletionPort, "wtf");
        SetLastError(0);
        formartIocpevent(pEvent, this, socket_handler, SO_TCPSEND);
        if (0 == m_sendStream.size()) {
            if (m_nStatus == SS_WAITCLOSE) {
                return false;
            }

            if (!PostQueuedCompletionStatus(hCompletionPort, 0, socket_handler, (LPWSAOVERLAPPED)pEvent)) {
                // some error must be deal.
                TASSERT(false, "PostQueuedCompletionStatus error %d", ::GetLastError());
                return false;
            } 
        } else {
            m_sendStream.LockWrite();
            pEvent->wbuf.buf = (char *)m_sendStream.buff();
            pEvent->wbuf.len = (m_sendStream.size()>=Configmgr::getInstance()->GetCoreConfig()->sNetSendSize)?(Configmgr::getInstance()->GetCoreConfig()->sNetSendSize):(m_sendStream.size());
            s32 res = WSASend(socket_handler, &pEvent->wbuf, 1, NULL, 0, (LPWSAOVERLAPPED)pEvent, NULL);
            m_sendStream.FreeWrite();

            if (SOCKET_ERROR == res && WSA_IO_PENDING == (pEvent->code = WSAGetLastError()) ) {
                return false;
            }
        }

        return true;
    }
}