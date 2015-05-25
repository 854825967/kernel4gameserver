#ifndef __SPipe_h__
#define __SPipe_h__
#include "Pipe.h"

class SPipe : public Pipe {
public:
    SPipe();

    inline void Send(const void * pContext, const s32 nSize) {
        TASSERT(false, "server pipe can send nothing.");
    }

    bool DoAccept();

public:
    HANDLE m_hCompletionPort;
    struct iocp_event * m_pAcceptEvent;
    ITcpServer * m_pHost;
};

//extern tlib::TPool<Pipe, true, 4096> g_poolPipe;

#endif //__SPipe_h__