#ifndef __Header_h__
#define __Header_h__
#include "MultiSys.h"

#include "TPool.h"
#include "TQueue.h"

enum eSocketOpt {
    SO_ACCEPT,
    SO_TCPRECV,
    SO_TCPSEND,
    SO_CONNECT,

    //FOR UDP
    //SO_UDPIO,
};

enum eSocketStatus {
    SS_UNINITIALIZE = 0,
    SS_ESTABLISHED,
    SS_WAITCLOSE,
};

class CPipe;

enum {
    IO_EVENT_TYPE_BREAK = 0,
    IO_EVENT_TYPE_RECV = 1,
    IO_EVENT_TYPE_CONNECTED = 2,
    IO_EVENT_TYPE_CONNECTFAILD = 3,

    //ADD BEFOR THIS
    IO_EVENT_TYPE_COUNT
};

struct ioevent {
    s8 type;
    CPipe * pCPipe;
};

typedef tlib::TQueue<ioevent *, false, 4096> IOEVENT_QUEUE;
typedef tlib::TPool<ioevent, true> IOEVENT_POOL;
#define BUFF_SIZE (16 * 1024)

#endif //__Header_h__
