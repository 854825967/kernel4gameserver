
/* 
 * File:   SPipe.cpp
 * Author: alax
 * 
 * Created on May 18, 2015, 2:09 PM
 */
#include "SPipe.h"
#include "CPipe.h"
#include "Kernel.h"
#include "epoller.h"
SPIPE_POOL SPipe::s_oSPipePool;

SPipe * SPipe::Create() {
    return s_oSPipePool.Create();
}

void SPipe::Release() {
    Clear();
    s_oSPipePool.Recover(this);
}

void SPipe::Send(const void * pContext, const s32 nSize) {
    TASSERT(false, "spipe can't call send");
}

void SPipe::DoAccept() {
    struct sockaddr_in addr;
    socklen_t len = sizeof (addr);
    s64 lSocket = -1;
    while ((lSocket = accept(m_lSocketHandler, (sockaddr *) & addr, &len)) >= 0) {
        if (setnonblocking(lSocket)) {
            CPipe * pCPipe = CPipe::Create();
            TASSERT(pCPipe, "check pool template");
            memcpy(&pCPipe->m_oAddr, &addr, sizeof (pCPipe->m_oAddr));

            tcore::ITcpSession * pSession = m_pHost->MallocConnection(Kernel::getInstance());
            TASSERT(pSession, "session point null");
            pCPipe->m_lSocketHandler = lSocket;
            pCPipe->m_nStatus = SS_ESTABLISHED;

            epoller * pEpoller = (epoller *) epoller::getInstance();
            s64 lEpollFD = pEpoller->BalancingWorker()->GetEpollFD();
            pCPipe->Relate(pSession, lSocket, lEpollFD);

            epoll_event ev;
            ev.data.ptr = &pCPipe->m_oEvent;
            ev.events = EPOLLIN | EPOLLOUT | EPOLLET;

            s32 res = epoll_ctl(lEpollFD, EPOLL_CTL_ADD, lSocket, &ev);
            TASSERT(0 == res, "epoll_ctl error %d", tools::GetLastErrno());
            if (0 == res) {
                pCPipe->m_nStatus == SS_ESTABLISHED;
                pSession->OnConnected(Kernel::getInstance());
            } else {
                pCPipe->Release();
            }
        } else {
            ECHO("socket opt error %d", tools::GetLastErrno());
            break;
        }
    }

}
