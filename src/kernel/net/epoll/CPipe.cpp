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

void CPipe::Send(const void * pContext, const s32 nSize) {
    TASSERT(m_nStatus == SS_ESTABLISHED, "wtf");
    if (m_nStatus != SS_ESTABLISHED) {
        return;
    }
    
    m_oSendStream.in(pContext, nSize);
    m_oSendStream.LockRead();
    if (m_bNeedSend) {
        static s32 nSendSize = Configmgr::getInstance()->GetCoreConfig()->sNetSendSize;
        while (true) {
            s32 nLen = m_oSendStream.size();
            if (nLen > 0) {
                s32 nSendLen = send(m_lSocketHandler, m_oSendStream.buff(), (nLen >= nSendSize)?nSendSize:nLen, 0);
                if (nSendLen > 0) {
                    m_oSendStream.out(nSendLen);
                } else if (nSendLen == -1 && EAGAIN == tools::GetLastErrno()) {
                    m_bNeedSend = false;
                    m_oSendStream.FreeRead();
                    break;
                } else {
                    ECHO("send error %d", tools::GetLastErrno());
                    DoClose();
                    m_pHost->OnDisconnect(Kernel::getInstance());
                    m_pHost->m_pPipe = NULL;
                    Release();
                    m_oSendStream.FreeRead();
                    break;
                }
            } else if (m_nStatus == SS_WAITCLOSE) {
                DoClose();
                m_pHost->OnDisconnect(Kernel::getInstance());
                m_pHost->m_pPipe = NULL;
                Release();
                m_oSendStream.FreeRead();
                break;
            } else {
                m_bNeedSend = true;
                m_oSendStream.FreeRead();
                break;
            }
        }
    }
}

void CPipe::DoClose() {
    TASSERT(m_nStatus != SS_UNINITIALIZE, "wtf");
    if (m_nStatus != SS_UNINITIALIZE) {
        m_nStatus = SS_UNINITIALIZE;
        s32 res = epoll_ctl(m_lEpollFD, EPOLL_CTL_DEL, m_lSocketHandler, NULL);
        TASSERT(0 == res, "epoll_ctl del error %d", tools::GetLastErrno());
        close(m_lSocketHandler);
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
            ECHO("link over or recv error %d", tools::GetLastErrno());
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

s32 CPipe::DoConnect() {
    m_pHost->OnConnected(Kernel::getInstance());
}

s32 CPipe::DoSend() {
    static s32 nSize = Configmgr::getInstance()->GetCoreConfig()->sNetSendSize;
    
    m_oSendStream.LockRead();
    while (true) {
        s32 nLen = m_oSendStream.size();
        if (nLen > 0) {
            s32 nSendLen = send(m_lSocketHandler, m_oSendStream.buff(), (nLen >= nSize)?nSize:nLen, 0);
            if (nSendLen > 0) {
                m_oSendStream.out(nSendLen);
            } else if (nSendLen == -1 && EAGAIN == tools::GetLastErrno()) {
                m_oSendStream.FreeRead();
                return IO_EVENT_TYPE_COUNT;
            } else {
                ECHO("send error %d", tools::GetLastErrno());
                m_oSendStream.FreeRead();
                DoClose();
                return IO_EVENT_TYPE_BREAK;
            }
            
        } else if (m_nStatus == SS_WAITCLOSE) {
            DoClose();
            m_oSendStream.FreeRead();
            return IO_EVENT_TYPE_BREAK;
        } else {
            m_bNeedSend = true;
            m_oSendStream.FreeRead();
            return IO_EVENT_TYPE_COUNT;
        }
    }
    
}
