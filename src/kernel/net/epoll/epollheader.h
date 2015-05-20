/* 
 * File:   epoller_header.h
 * Author: alax
 *
 * Created on January 14, 2015, 5:03 PM
 */

#ifndef __epoller_header_h__
#define	__epoller_header_h__
#include "Header.h"
#include "TPool.h"
#include "TQueue.h"
#include "CThread.h"
#include "CLock.h"
#include "CHashMap.h"
#include "IKernel.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define EPOLL_DESC_COUNT 2048
#define EPOLLER_EVENTS_COUNT 128

#ifdef __cplusplus
extern "C" {
#endif
    static inline bool setnonblocking(s64 fd) {
        return fcntl((fd), F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == 0;
    }

    struct epollerEvent {
        s8 type;
        void * pData;
    };
    
#ifdef	__cplusplus
}
#endif

typedef tlib::TPool<epollerEvent> EPOLLEREVENT_PO0L;

#endif	/* __epoller_header_h__ */
