#ifndef __Pipe_h__
#define __Pipe_h__
#include "IKernel.h"
#include "TStream.h"
#include "Header.h"
using namespace tlib;
using namespace tcore;

class Pipe : public IPipe {
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
        m_lSocketHandler = INVALID_SOCKET;
        tools::SafeMemset(&m_oAddr, sizeof(m_oAddr), 0, sizeof(m_oAddr));
    }

public:
    s8 m_nStatus;
    s64 m_lSocketHandler;
    struct sockaddr_in m_oAddr;
};


#endif //__Pipe_h__
