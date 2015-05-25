/* 
 * File:   CPipe.cpp
 * Author: alax
 * 
 * Created on May 18, 2015, 1:42 PM
 */

#include "CPipe.h"
#include "configmgr/Configmgr.h"
#include "Kernel.h"

CPIPE_POOL CPipe::s_oCPipePool;

CPipe::CPipe() : m_lEpollFD(-1), m_pHost(NULL), m_bNeedSend(true) {
    m_oEvent.type = SO_CONNECT;
    m_oEvent.pData = this;
}

CPipe::~CPipe() {

}

CPipe * CPipe::Create() {
    return s_oCPipePool.Create();
}

void CPipe::Release() {
    TASSERT(m_nStatus == SS_UNINITIALIZE, "wtf");
    Clear();
    s_oCPipePool.Recover(this);
}

    
void CPipe::PostOPTInEpollQueue(s32 opt) {
    epoll_event ev;
    ev.data.ptr = &m_oEvent;
    ev.events = opt;
    s32 res = epoll_ctl(m_lEpollFD, EPOLL_CTL_MOD, m_lSocketHandler, &ev);
    TASSERT(0 == res, "wtf");
}

void CPipe::Close() {
    m_nStatus = SS_WAITCLOSE;
    m_oSendStream.LockRead();
    if (m_bNeedSend) {
        PostOPTInEpollQueue(EPOLLOUT | EPOLLONESHOT);
    }
    m_oSendStream.FreeRead();
}

void CPipe::Send(const void * pContext, const s32 nSize) {
    TASSERT(m_nStatus == SS_ESTABLISHED, "wtf");
    if (m_nStatus != SS_ESTABLISHED) {
        return;
    }
    
    m_oSendStream.in(pContext, nSize);
    m_oSendStream.LockRead();
    if (m_bNeedSend) {
        while (true) {
            s32 nLen = m_oSendStream.size();
            if (nLen > 0) {
                s32 nSendLen = send(m_lSocketHandler, m_oSendStream.buff(), nLen, 0);
                if (nSendLen > 0) {
                    m_oSendStream.out(nSendLen);
                } else if (nSendLen == -1 && EAGAIN == tools::GetLastErrno()) {
                    m_bNeedSend = false;
                    m_oSendStream.FreeRead();
                    break;
                } else {
                    m_oSendStream.FreeRead();
                    ECHO("send error %d", tools::GetLastErrno());
                    PostOPTInEpollQueue(EPOLLERR | EPOLLHUP | EPOLLRDHUP);
                    break;
                }
            } else if (m_nStatus == SS_WAITCLOSE) {
                m_oSendStream.FreeRead();
                PostOPTInEpollQueue(EPOLLERR | EPOLLHUP | EPOLLRDHUP);
                break;
            } else {
                m_bNeedSend = true;
                m_oSendStream.FreeRead();
                break;
            }
        }
    } else {
        m_oSendStream.FreeRead();
    }
}

void CPipe::DoClose() {
    TASSERT(m_nStatus != SS_UNINITIALIZE && m_lSocketHandler != -1, "wtf");
    if (m_nStatus != SS_UNINITIALIZE) {
        m_nStatus = SS_UNINITIALIZE;
        s32 res = epoll_ctl(m_lEpollFD, EPOLL_CTL_DEL, m_lSocketHandler, NULL);
        TASSERT(0 == res, "epoll_ctl del error %d", tools::GetLastErrno());
        linger _linger;
        _linger.l_onoff = 0;
        _linger.l_linger = 0;
        setsockopt(m_lSocketHandler, SOL_SOCKET, SO_LINGER, (const char *)&_linger, sizeof(_linger));
        close(m_lSocketHandler);
        m_lSocketHandler = -1;
    }
}

s32 CPipe::DoRecv() {
    static s32 nSize = Configmgr::getInstance()->GetCoreConfig()->sNetRecvSize;
    char buff[nSize];
    s32 nRecvSize = 0;
    while (true) {
        s32 len = recv(m_lSocketHandler, buff, nSize, 0);
        if (len<0 && tools::GetLastErrno() == EAGAIN) {
            break;
        }
        
        if (len <= 0) {
            DoClose();
            return IO_EVENT_TYPE_BREAK;
        }
        
        nRecvSize += len;
        m_oRecvStream.in(buff, len);
    }
    
    if (nRecvSize > 0) {
        return IO_EVENT_TYPE_RECV;
    }
    
    return IO_EVENT_TYPE_COUNT;
}

void CPipe::DoConnect() {
    epoll_event ev;
    ev.data.ptr = &m_oEvent;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
    
    m_nStatus = SS_ESTABLISHED;
    s32 res = epoll_ctl(m_lEpollFD, EPOLL_CTL_ADD, m_lSocketHandler, &ev);
    TASSERT(0 == res, "epoll ctl error %d", tools::GetLastErrno());
    if (0 == res) {
        m_pHost->OnConnected(Kernel::getInstance());
    } else {
        m_nStatus = SS_UNINITIALIZE;
        close(m_lSocketHandler);
        m_pHost->m_pPipe = NULL;
        m_pHost->OnConnectFailed(Kernel::getInstance());
        Release();
    }
}

s32 CPipe::DoSend() {
    while (true) {
        m_oSendStream.LockRead();
        s32 nLen = m_oSendStream.size();
        if (nLen > 0) {
            s32 nSendLen = send(m_lSocketHandler, m_oSendStream.buff(), nLen, 0);
            if (nSendLen > 0) {
                m_oSendStream.out(nSendLen);
                m_oSendStream.FreeRead();
            } else if (nSendLen == -1 && EAGAIN == tools::GetLastErrno()) {
                m_oSendStream.FreeRead();
                return IO_EVENT_TYPE_COUNT;
            } else {
                m_oSendStream.FreeRead();
                ECHO("send error %d", tools::GetLastErrno());
                return IO_EVENT_TYPE_BREAK;
            }
        } else if (m_nStatus == SS_WAITCLOSE) {
            m_oSendStream.FreeRead();
            return IO_EVENT_TYPE_BREAK;
        } else {
            m_bNeedSend = true;
            m_oSendStream.FreeRead();
            return IO_EVENT_TYPE_COUNT;
        }
    }
    
}
