#ifndef __Tools_h__
#define __Tools_h__

#include "MultiSys.h"
#include <sys/time.h>

namespace tools {
#ifdef __cplusplus
    extern "C" {
#endif

        inline s64 GetTimeMillisecond() {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return tv.tv_sec * 1000 + tv.tv_usec / 1000;
        }
#ifdef __cplusplus
    }
#endif
}

#endif //__Tools_h__
