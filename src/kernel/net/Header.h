#ifndef __Header_h__
#define __Header_h__

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

#define BUFF_SIZE (16 * 1024)

#endif //__Header_h__
