#ifndef __CPipe_h__
#define __CPipe_h__
#include "Pipe.h"
#include "IKernel.h"
#include "TStream.h"
using namespace tlib;
using namespace tcore;
struct iocp_event;
class CPipe : public Pipe {
public:
    CPipe();
    ~CPipe();

    virtual void Send(const void * pContext, const s32 nSize);

    void In(const void * pContext, const s32 nSize);
    bool Out(const s32 nSize);

    bool DoConnect(const char * ip, const s32 port);
    bool DoRecv();

    void Clear();

public:
    HANDLE m_hCompletionPort;
    ITcpSession * m_pHost;
    bool m_bNeedPostOPT;
    struct iocp_event * m_pRecvEvent;
    struct iocp_event * m_pSendEvent;
    TStream<BUFF_SIZE, true> m_oRecvStream;
    TStream<BUFF_SIZE, true> m_oSendStream;

    s32 m_nTestCount;
};

#endif //__CPipe_h__
