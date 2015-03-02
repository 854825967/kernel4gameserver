#include "IKernel.h"
#include "epoller.h"
#include <arpa/inet.h>
namespace tcore {

    static s32 s_index = 0;

    void ITcpSocket::DoIO(s32 flags, void* pContext) {
        epoller * pEpoller = (epoller *) pContext;

        if (m_nStatus == SS_WAITCLOSE) {
            shutdown(socket_handler, SHUT_RD);
        }

        if (flags & EPOLLIN && m_nStatus == SS_ESTABLISHED) {
            //read
            char buff[RECV_BUFF_LEN];
            memset(buff, 0, sizeof (buff));
            s32 recvLen = 0;
            bool bRecv = false;
            bool bError = false;
            bool bClose = false;
            while (true) {
                memset(buff, 0, sizeof (buff));
                s32 len = recv(socket_handler, buff, RECV_BUFF_LEN, 0);
                if (len < 0 && errno == EAGAIN) {
                    //no buff for read
                    break;
                }

                if (m_nStatus == SS_WAITCLOSE) {
                    break;
                }

                if (len < 0) {
                    bError = true;
                    ECHO("recv error %s", strerror(errno));
                    m_nStatus = SS_UNINITIALIZE;
                    pEpoller->epoller_CTL(EPOLL_CTL_DEL, socket_handler, NULL);
                    shut_socket(socket_handler);
                    break;
                } else if (len == 0) {
                    bClose = true;
                    //socket closed
                    m_nStatus = SS_UNINITIALIZE;
                    pEpoller->epoller_CTL(EPOLL_CTL_DEL, socket_handler, NULL);
                    shut_socket(socket_handler);
                    break;
                } else {
                    bRecv = true;
                    recvLen += len;
                    m_recvStream.in(buff, len);
                }
            }

            if (bRecv) {
                epoller_data * p = g_EpollerDataPool.Create();
                p->opt = SO_TCPIO;
                p->user_ptr = this;
                p->code = 0;
                p->len = recvLen;
                pEpoller->AddIO(p);
            }

            if (bClose) {
                epoller_data * p = g_EpollerDataPool.Create();
                p->opt = SO_TCPIO;
                p->user_ptr = this;
                p->code = 0;
                p->len = 0;
                pEpoller->AddIO(p);
                return;
            }

            if (bError) {
                epoller_data * p = g_EpollerDataPool.Create();
                p->opt = SO_TCPIO;
                p->user_ptr = this;
                p->code = -1;
                p->len = 0;
                pEpoller->AddIO(p);
                return;
            }

        }

        if (flags & EPOLLOUT && m_nStatus != SS_UNINITIALIZE) {
            //write
            while (m_sendStream.size() > 0) {
                m_sendStream.LockWrite();
                s32 sendlen = send(socket_handler, m_sendStream.buff(), m_sendStream.size(), 0);
                m_sendStream.FreeWrite();
                if (sendlen > 0) {
                    ECHO("send buff len %d", sendlen);
                    m_sendStream.out(sendlen);
                } else if (sendlen == -1) {
                    if (EAGAIN == errno) {
                        break;
                    }

                    if (ECONNRESET == errno) {
                        TASSERT(false, "send error");
                        epoller_data * p = g_EpollerDataPool.Create();
                        p->opt = SO_TCPIO;
                        p->user_ptr = this;
                        p->code = -1;
                        p->len = 0;
                        m_nStatus = SS_UNINITIALIZE;
                        pEpoller->epoller_CTL(EPOLL_CTL_DEL, socket_handler, NULL);
                        shut_socket(socket_handler);
                        pEpoller->AddIO(p);
                        return;
                    }
                }
            }

            if (m_sendStream.size() == 0 && m_nStatus == SS_WAITCLOSE) {
                ECHO("tcpsocket closed");
                epoller_data * p = g_EpollerDataPool.Create();
                p->opt = SO_TCPIO;
                p->user_ptr = this;
                p->code = 0;
                p->len = 0;
                m_nStatus = SS_UNINITIALIZE;
                pEpoller->epoller_CTL(EPOLL_CTL_DEL, socket_handler, NULL);
                shut_socket(socket_handler);
                pEpoller->AddIO(p);
                return;
            }
        }

        if ((flags & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) && m_nStatus != SS_UNINITIALIZE) {
            // error
            ECHO("io error %s", strerror(errno));
            epoller_data * p = g_EpollerDataPool.Create();
            p->opt = SO_TCPIO;
            p->user_ptr = this;
            p->code = -1;
            p->len = 0;
            m_nStatus = SS_UNINITIALIZE;
            pEpoller->epoller_CTL(EPOLL_CTL_DEL, socket_handler, NULL);
            shut_socket(socket_handler);
            pEpoller->AddIO(p);
        }
    }

    void ITcpSocket::DoConnect(s32 flags, void* pContext) {
        epoller * pEpoller = (epoller *) pContext;
        s32 status = -1;
        socklen_t slen;
        if (!(flags & EPOLLOUT)
                || 0 > getsockopt(socket_handler, SOL_SOCKET, SO_ERROR, &status, &slen)
                || 0 != status) {
            ECHO("getsockopt error %s", strerror(errno));
            Error(SO_CONNECT, -1);

            bool res = pEpoller->epoller_CTL(EPOLL_CTL_DEL, socket_handler, NULL);
            TASSERT(res, strerror(errno));
            shut_socket(socket_handler);
        } else {
//            ECHO("non-blocking connect success");
            m_nStatus = SS_ESTABLISHED;
            Error(SO_CONNECT, 0);

            bool res = pEpoller->epoller_CTL(EPOLL_CTL_DEL, socket_handler, NULL);
            TASSERT(res, strerror(errno));

            epoller_data * data = g_EpollerDataPool.Create();
            data->user_ptr = this;
            data->opt = SO_TCPIO;
            data->index = s_index++;

            epoll_event ev;
            ev.events = EPOLLIN | EPOLLOUT;
            ev.data.ptr = data;

            res = pEpoller->epoller_CTL(EPOLL_CTL_ADD, socket_handler, &ev);
            TASSERT(res, strerror(errno));
            Connected();
        }

    }

    void ITcpServer::DoAccept(s32 flags, void * pContext) {
        epoller * pEpoller = (epoller *) pContext;
        if (flags == EPOLLIN) {
            struct sockaddr_in addr;
            socklen_t len = sizeof (addr);
            s32 handler = accept(socket_handler, (sockaddr *) & addr, &len);
            while (handler >= 0) {
                if (setnonblocking(handler)) {
                    ITcpSocket * pSocket = MallocConnection();
                    TASSERT(pSocket, "tcpsocket point is null");
                    memcpy(&pSocket->m_addr, &addr, sizeof (pSocket->m_addr));
                    SafeSprintf(pSocket->ip, sizeof (pSocket->ip), inet_ntoa(addr.sin_addr));
                    pSocket->port = ntohs(addr.sin_port);
                    pSocket->socket_handler = handler;
                    pSocket->m_nStatus = SS_ESTABLISHED;
//                    ECHO("remote ip:%s port:%d", pSocket->ip, pSocket->port);

                    epoller_data * data = g_EpollerDataPool.Create();
                    data->user_ptr = pSocket;
                    data->opt = SO_TCPIO;
                    data->index = s_index++;

                    epoll_event ev;
                    ev.events = EPOLLIN | EPOLLOUT;
                    ev.data.ptr = data;

                    bool res = pEpoller->epoller_CTL(EPOLL_CTL_ADD, pSocket->socket_handler, &ev);
                    TASSERT(res, strerror(errno));
                    pSocket->Connected();
                } else {
                    ECHO("setnonblock error %s", strerror(errno));
                    shut_socket(handler);
                }

                handler = accept(socket_handler, (sockaddr *) & addr, &len);
            }

//            ECHO("accept over");
        } else {
            ECHO("bad accept %s", strerror(errno));
            Error(SO_ACCEPT, errno);
            bool res = pEpoller->epoller_CTL(EPOLL_CTL_DEL, socket_handler, NULL);
            TASSERT(res, strerror(errno));
            shut_socket(socket_handler);
        }
    }
}
