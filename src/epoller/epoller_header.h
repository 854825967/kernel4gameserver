/* 
 * File:   epoller_header.h
 * Author: alax
 *
 * Created on January 14, 2015, 5:03 PM
 */

#ifndef __epoller_header_h__
#define	__epoller_header_h__
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
#define EPOLLER_EVENTS_COUNT 512

#ifdef __cplusplus
extern "C" {
#endif
    struct epoller_event {
        s8 opt;
        union {
            s32 index;
            s32 len;
        };
        union {
            s32 flags;
            s32 code;
        };
        union {
            void * user_ptr;
            s64 __nothing;
        };
    };

    inline void shut_socket(s64 fd) {
        //shutdown(fd, SHUT_RDWR);
        close(fd);
    }

    inline bool setnonblocking(s64 fd) {
        return fcntl((fd), F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == 0;
    }

#ifdef	__cplusplus
}
#endif
extern tlib::TPool<struct epoller_event, true, 128> g_EpollerDataPool;

#endif	/* __epoller_header_h__ */
