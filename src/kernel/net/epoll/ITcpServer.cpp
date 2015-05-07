#include "IKernel.h"
#include "epoller.h"
#include "epoller_worker.h"
#include "Kernel.h"
#include "configmgr/Configmgr.h"
#include <arpa/inet.h>
namespace tcore {
    static s32 s_index = 0;
    void ITcpSocket::DoIO(void * p, void* pContext) {
        epoller_event * pEvent = (epoller_event *)p;
        epoller_worker * pWoker = (epoller_worker *) pContext;
        TASSERT(m_nStatus != SS_UNINITIALIZE, "wtf");
        if ((pEvent->flags & EPOLLIN) && SS_ESTABLISHED == m_nStatus) {
            //read
            const s32 nReadSize = Configmgr::getInstance()->GetCoreConfig()->sNetRecvSize;
            char buff[nReadSize];
            memset(buff, 0, sizeof (buff));
            s32 recvLen = 0;

            while (SS_ESTABLISHED == m_nStatus) {
                s32 len = recv(socket_handler, buff, nReadSize, 0);
                if (len < 0 && errno == EAGAIN) {
                    break;
                }

                if (len <= 0) {
                    ECHO("link over or recv error %s", strerror(errno));
                    m_nStatus = SS_WAITCLOSE;
                    break;
                } else {
                    recvLen += len;
                    m_recvStream.in(buff, len);
                }
            }

            if (recvLen > 0) {
                epoller_event * p = g_EpollerDataPool.Create();
                p->opt = SO_TCPRECV;
                p->user_ptr = this;
                p->code = 0;
                p->len = recvLen;
                pWoker->AddEvent(p);
            }
        }

        //send
        if ( (pEvent->flags & EPOLLOUT) && m_nStatus != SS_UNINITIALIZE) {
            s32 nSendLen = Configmgr::getInstance()->GetCoreConfig()->sNetSendSize;
            //write
            while (m_sendStream.size() > 0) {
                m_sendStream.LockWrite();
                s32 sendlen = send(socket_handler, m_sendStream.buff(), nSendLen, 0);
                m_sendStream.FreeWrite();
                if (sendlen > 0) {
                    m_sendStream.out(sendlen);
                    if (m_sendStream.size() == 0 && m_nStatus == SS_WAITCLOSE) {
                        TASSERT(false, "send error");
                        pWoker->SendDisconnectEvent(pEvent);
                        return;
                    }
                } else if (sendlen == -1) {
                    if (EAGAIN == errno) {
                        break;
                    }

                    pWoker->SendDisconnectEvent(pEvent);
                    return;
                }
            }
            
            if (m_sendStream.size() == 0) {
                if (m_nStatus == SS_WAITCLOSE) {
                    pWoker->SendDisconnectEvent(pEvent);
                }
            }
        }

        if ((pEvent->flags & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) && m_nStatus != SS_UNINITIALIZE) {
            // error
            ECHO("io error %s", strerror(errno));
            pWoker->SendDisconnectEvent(pEvent);
        }
    }

    void ITcpSocket::DoConnect(void * p, void* pContext) {
        epoller * pEpoller = (epoller *) pContext;
        epoller_event * pEvent = (epoller_event *)p;
        s32 status = -1;
        socklen_t slen;
        if (!(pEvent->flags & EPOLLOUT)
                || 0 > getsockopt(socket_handler, SOL_SOCKET, SO_ERROR, &status, &slen)
                || 0 != status) {
            TASSERT(false, "getsockopt error %s", strerror(errno));
            Error(Kernel::getInstance(), SO_CONNECT, NULL, strerror(errno));
            shut_socket(socket_handler);
        } else {
            //sth. must be deal
            epoller_worker * pWorker = pEpoller->BalancingWorker();
            if (pWorker->RelateSocketClient(this->socket_handler, this)) {
                m_nStatus = SS_ESTABLISHED;
                Connected(Kernel::getInstance());
            } else {
                TASSERT(false, "wtf");
                Error(Kernel::getInstance(), SO_CONNECT, NULL, strerror(errno));
            }
        }
        g_EpollerDataPool.Recover(pEvent);
    }

    void ITcpServer::DoAccept(void * p, void * pContext) {
        epoller * pEpoller = (epoller *) pContext;
        epoller_event * pEvent = (epoller_event *)p;
        if (pEvent->flags == EPOLLIN) {
            struct sockaddr_in addr;
            socklen_t len = sizeof (addr);
            s32 handler = -1;
            while ( (handler = accept(socket_handler, (sockaddr *) & addr, &len) ) >= 0) {
                if (setnonblocking(handler)) {
                    ITcpSocket * pSocket = MallocConnection(Kernel::getInstance());
                    TASSERT(pSocket, "tcpsocket point is null");
                    memcpy(&pSocket->m_addr, &addr, sizeof (pSocket->m_addr));
                    SafeSprintf(pSocket->ip, sizeof (pSocket->ip), inet_ntoa(addr.sin_addr));
                    pSocket->port = ntohs(addr.sin_port);
                    pSocket->socket_handler = handler;
                    pSocket->m_nStatus = SS_ESTABLISHED;

                    epoller_worker * pWorker = pEpoller->BalancingWorker();
                    if (pWorker->RelateSocketClient(pSocket->socket_handler, pSocket)) {
                        pSocket->Connected(Kernel::getInstance());
                    } else {
                        Error(Kernel::getInstance(), SO_ACCEPT, pSocket, "RelateSocketClient error");
                        TASSERT(false, "wtf");
                    }
                } else {
                    ECHO("setnonblock error %s", strerror(errno));
                    shut_socket(handler);
                }
            }
        } else {
            ECHO("bad accept %s", strerror(errno));
            pEpoller->remove_handler(socket_handler);
            shut_socket(socket_handler);
            g_EpollerDataPool.Recover(pEvent);
            Error(Kernel::getInstance(), SO_ACCEPT, NULL, "DoAccept error");
        }
    }
}
