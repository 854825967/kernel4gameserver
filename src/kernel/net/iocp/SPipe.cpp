#include "SPipe.h"
#include "iocphead.h"
#include "configmgr/Configmgr.h"

SPipe::SPipe() {
//     IConfigmgr * m_pConfigMgr = Configmgr::getInstance();
//     TASSERT(m_pConfigMgr, "where is config manager");
    m_pAcceptEvent = NEW iocp_event(256);
}

bool SPipe::DoAccept() {
    formartIocpevent(m_pAcceptEvent, this, INVALID_SOCKET, SO_ACCEPT);
    if (INVALID_SOCKET == (m_pAcceptEvent->socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))) {
        closesocket(m_lSocketHandler);
        return false;
    }

    s32 res = g_pFunAcceptEx(
        m_lSocketHandler,
        m_pAcceptEvent->socket,
        m_pAcceptEvent->pBuff,
        0,
        sizeof(struct sockaddr_in) + 16,
        sizeof(struct sockaddr_in) + 16,
        &m_pAcceptEvent->dwBytes,
        (LPOVERLAPPED)m_pAcceptEvent
        );

    s32 err = WSAGetLastError();
    if (res == FALSE && err != WSA_IO_PENDING) {
        closesocket(m_lSocketHandler);
        TASSERT(false, "AcceptEx error %d", err);
    }

    return true;
}
