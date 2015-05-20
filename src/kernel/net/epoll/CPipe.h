/* 
 * File:   CPipe.h
 * Author: alax
 *
 * Created on May 18, 2015, 1:42 PM
 */

#ifndef __CPipe_h__
#define	__CPipe_h__

#include "Pipe.h"
#include "TPool.h"
#include "epollheader.h"

typedef tlib::TPool<CPipe, false> CPIPE_POOL;

class CPipe : public Pipe {
public:
    static CPipe * Create();
    void Release();
    
    CPipe();
    virtual ~CPipe();
    
    virtual void Send(const void * pContext, const s32 nSize);
    
    s32 DoRecv();
    s32 DoSend();
    void DoClose();
    void DoConnect();
    
    
    inline void Clear() {
        Pipe::Clear();
        m_oRecvStream.clear();
        m_oSendStream.clear();
        m_lEpollFD = -1;
        m_pHost = NULL;
        m_bNeedSend = true;
    }
    
    inline void Relate(tcore::ITcpSession * pHost, s64 lSocket, s64 lEpollFD) {
        TASSERT(pHost != NULL, "wtf where is host");
        m_pHost = pHost;
        pHost->m_pPipe = this;
        m_lEpollFD = lEpollFD;
        m_lSocketHandler = lSocket;
    }
    
public:
    tcore::ITcpSession * m_pHost;
    s64 m_lEpollFD;
    bool m_bNeedSend;
    tlib::TStream<BUFF_SIZE, true> m_oRecvStream;
    tlib::TStream<BUFF_SIZE, true> m_oSendStream;
    epollerEvent m_oEvent;
    static CPIPE_POOL s_oCPipePool;
};

#endif	/* __CPipe_h__ */

