/* 
 * File:   header.h
 * Author: alax
 *
 * Created on January 14, 2015, 5:03 PM
 */

#ifndef HEADER_H
#define	HEADER_H
#include <fcntl.h>
#include "TPool.h"

#define RECV_BUFF_LEN 4096
#define SEND_BUFF_LEN 4096
#define EPOLL_DESC_COUNT 4096
#define EPOLLER_EVENTS_COUNT 512
#define EPOLLER_DATA_COUNT 4096

#ifdef __cplusplus
extern "C" {
#endif

    struct epoller_data {
        s16 opt;

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

    inline void shut_socket(int fd) {
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }

    inline bool setnonblocking(int fd) {
        return fcntl((fd), F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == 0;
    }

#ifdef	__cplusplus
}
#endif

extern tlib::TPool<struct epoller_data, true, 128> g_EpollerDataPool;

#endif	/* HEADER_H */

