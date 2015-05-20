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

    //
    inline void Close() {m_nStatus = SS_WAITCLOSE;}


public:
    void Clear() {
        m_nStatus = SS_UNINITIALIZE;
        m_lSocketHandler = -1;
        tools::SafeMemset(&m_oAddr, sizeof(m_oAddr), 0, sizeof(m_oAddr));
    }

public:
    s8 m_nStatus;
    s64 m_lSocketHandler;
    struct sockaddr_in m_oAddr;
};

#endif	/* __Pipe_h__ */

