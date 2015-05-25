/* 
 * File:   Pipe.h
 * Author: alax
 *
 * Created on May 18, 2015, 1:51 PM
 */

#ifndef __Pipe_h__
#define __Pipe_h__

#include "IKernel.h"
#include "Header.h"
#include "Tools.h"

class Pipe : public tcore::IPipe {
public:
    virtual ~Pipe() {}

    Pipe() {
        Clear();
    }

    void Clear() {
        m_nStatus = SS_UNINITIALIZE;
        m_lSocketHandler = -1;
        tools::SafeMemset(&m_oAddr, sizeof(m_oAddr), 0, sizeof(m_oAddr));
    }
    
    inline void SetStatus(s8 status) {m_nStatus = status;};
    inline s8 GetStatus() {return m_nStatus;}
    
    inline void SetSocketHandler(s64 handle) {m_lSocketHandler = handle;}
    inline s64 GetSocketHandler() {return m_lSocketHandler;}

    inline void SetSockAddr(const struct sockaddr_in & addr) {tools::SafeMemcpy(&m_oAddr, sizeof(m_oAddr), &addr, sizeof(addr));}
    inline struct sockaddr_in & GetSockAddr() {return m_oAddr;}
    
protected:
    s8 m_nStatus;
    s64 m_lSocketHandler;
    struct sockaddr_in m_oAddr;
};

#endif	/* __Pipe_h__ */

