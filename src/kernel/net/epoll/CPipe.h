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
    
    void Close();
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
        SetSocketHandler(lSocket);
    }
    
    inline void SetHost(tcore::ITcpSession * pHost) {TASSERT(m_pHost == NULL, "wtf"); m_pHost = pHost;}
    inline tcore::ITcpSession * GetHost() {return m_pHost;}
    
    inline void SetEpollFD(s64 fd) {TASSERT(m_lEpollFD == -1, "wtf"); m_lEpollFD = fd;}
    inline s64 GetEpollFD() {return m_lEpollFD;}
    
    inline void SetNeedPostSendOPT(bool need) {m_bNeedSend = need;}
    inline bool IsNeedPostSendOPT() {return &m_bNeedSend;}
    
    inline const epollerEvent * GetEvent() {return &m_oEvent;}
    
    tlib::TStream<BUFF_SIZE, true> m_oRecvStream;
    tlib::TStream<BUFF_SIZE, true> m_oSendStream;
    
private:
    void PostOPTInEpollQueue(s32 opt);
    
private:
    tcore::ITcpSession * m_pHost;
    s64 m_lEpollFD;
    bool m_bNeedSend;
    epollerEvent m_oEvent;
    static CPIPE_POOL s_oCPipePool;
};

#endif	/* __CPipe_h__ */

