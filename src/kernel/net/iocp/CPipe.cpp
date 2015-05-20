#include "CPipe.h"
#include "iocphead.h"
#include "configmgr/Configmgr.h"

static s32 s_count = 0;

CPipe::CPipe() {
    IConfigmgr * m_pConfigMgr = Configmgr::getInstance();
    TASSERT(m_pConfigMgr, "where is config manager");
    m_pRecvEvent = NEW iocp_event(m_pConfigMgr->GetCoreConfig()->sNetRecvSize);
    m_pSendEvent = NEW iocp_event(0);
    Clear();

    m_nTestCount = 0;
}

CPipe::~CPipe() {
    delete m_pSendEvent;
    delete m_pRecvEvent;
}

bool CPipe::DoConnect(const char * ip, const s32 port) {
    formartIocpevent(m_pRecvEvent, this, m_lSocketHandler, SO_CONNECT);
    m_oAddr.sin_family = AF_INET;
    m_oAddr.sin_port = htons(port);
    if((m_oAddr.sin_addr.s_addr = inet_addr(ip)) == INADDR_NONE) {
        ECHO("DoConnect error %d", ::GetLastError());
        closesocket(m_lSocketHandler);
        return false;
    }

    s32 res = g_pFunConnectEx(
        m_lSocketHandler,
        (struct sockaddr *)&m_oAddr,
        sizeof(struct sockaddr_in),
        NULL,
        0,
        &m_pRecvEvent->dwBytes,
        (LPOVERLAPPED)m_pRecvEvent
        );

    s32 err = WSAGetLastError();
    if (res == FALSE && err != WSA_IO_PENDING) {
        ECHO("DoConnect error %d", ::GetLastError());
        closesocket(m_lSocketHandler);
        return false;
    }

    return true;
}


void CPipe::Clear() {
    Pipe::Clear();
    m_hCompletionPort = NULL;
    m_bNeedPostOPT = true;
    m_oRecvStream.clear();
    m_oSendStream.clear();
    m_pRecvEvent->clear();
    m_pSendEvent->clear();
}

bool CPipe::DoRecv() {
    TASSERT(m_nStatus != SS_UNINITIALIZE, "WTF");
    formartIocpevent(m_pRecvEvent, this, m_lSocketHandler, SO_TCPRECV);
    if (m_nStatus == SS_ESTABLISHED) {
        TASSERT(m_pRecvEvent && m_lSocketHandler, "r u joking me");
        SetLastError(0);
        if (SOCKET_ERROR == WSARecv(m_lSocketHandler, &m_pRecvEvent->wbuf, 1, &m_pRecvEvent->dwBytes, &m_pRecvEvent->dwFlags, (LPWSAOVERLAPPED)m_pRecvEvent, NULL)) {
            m_pRecvEvent->code = GetLastError();
            if (ERROR_IO_PENDING != m_pRecvEvent->code) {
                return false;
            }
        }
        return true;
    } else if (m_bNeedPostOPT) {
        return false;
    }

    return true;
}

void CPipe::Send(const void * pContext, const s32 nSize) {
    m_oSendStream.in(pContext, nSize);
    m_oSendStream.LockRead();
    if (m_bNeedPostOPT) {
        formartIocpevent(m_pSendEvent, this, m_lSocketHandler, SO_TCPSEND);
        bool res = PostQueuedCompletionStatus(m_hCompletionPort, 0, m_lSocketHandler, (LPWSAOVERLAPPED)m_pSendEvent);
        TASSERT(res, "post error , wtf %d", ::GetLastError());
        m_bNeedPostOPT = false;
    }
    m_oSendStream.FreeRead();
}

void CPipe::In(const void * pContext, const s32 nSize) {
    m_oRecvStream.in(pContext, nSize);
}

bool CPipe::Out(const s32 nSize) {
    m_oSendStream.LockRead();
    m_oSendStream.out(nSize); 
    if (0 == m_oSendStream.size()) {
        m_bNeedPostOPT = true;
        m_oSendStream.FreeRead();
        if (SS_WAITCLOSE == m_nStatus) {
            return false;
        } else {
            return true;
        }
    }

    formartIocpevent(m_pSendEvent, this, m_lSocketHandler, SO_TCPSEND);

    s32 sLen = Configmgr::getInstance()->GetCoreConfig()->sNetSendSize;
    m_pSendEvent->wbuf.buf = (char *)m_oSendStream.buff();
    m_pSendEvent->wbuf.len = (m_oSendStream.size() >= sLen)?(sLen):(m_oSendStream.size());
    TASSERT(m_pSendEvent->wbuf.len > 0, "wtf");
    s32 res = WSASend(m_lSocketHandler, &m_pSendEvent->wbuf, 1, NULL, 0, (LPWSAOVERLAPPED)m_pSendEvent, NULL);

    if (SOCKET_ERROR == res && WSA_IO_PENDING != (m_pSendEvent->code = WSAGetLastError()) ) {
        m_oSendStream.FreeRead();
        return false;
    }

    m_oSendStream.FreeRead();
    return true;
}
