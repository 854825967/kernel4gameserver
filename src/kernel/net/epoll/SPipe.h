/* 
 * File:   SPipe.h
 * Author: alax
 *
 * Created on May 18, 2015, 2:09 PM
 */

#ifndef __SPipe_h__
#define	__SPipe_h__

#include "Pipe.h"
#include "TPool.h"
#include "epollheader.h"
class CPipe;
class SPipe;

typedef tlib::TPool<SPipe, false> SPIPE_POOL;

class SPipe : public Pipe {
public:

    static SPipe * Create();
    void Release();
    
    
    inline void Send(const void * pContext, const s32 nSize);
    
    inline void Relate(tcore::ITcpServer * pHost, s64 lSocket, s64 lEpollFD) {
        TASSERT(pHost != NULL, "wtf where is host");
        m_pHost = pHost;
        pHost->m_pPipe = this;
        m_lEpollFD = lEpollFD;
        m_lSocketHandler = lSocket;
    }
        
    void Clear() {
        Pipe::Clear();
        m_lEpollFD = -1;
        m_pHost = NULL;
    }
    
    SPipe() : m_lEpollFD(-1), m_pHost(NULL) {
        m_oEvent.type = SO_ACCEPT;
        m_oEvent.pData = this;
    }
    
    virtual ~SPipe() {
        
    }
    
    void DoAccept();
    
public:
    s64 m_lEpollFD;
    tcore::ITcpServer * m_pHost;
    epollerEvent m_oEvent;
    static SPIPE_POOL s_oSPipePool;
};

#endif	/* SPIPE_H */

