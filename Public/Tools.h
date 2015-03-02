#ifndef __Tools_h__
#define __Tools_h__

#include "MultiSys.h"
#include <sys/time.h>
#include <libgen.h>
#include <string>
using namespace std;

namespace tools {
#ifdef __cplusplus
    extern "C" {
#endif

        inline const char * GetAppPath() {
            static char * pStrPath = NULL;

            if (NULL == pStrPath) {
                pStrPath = NEW char[256];
                char link[256];
                memset(link, 0, 256);
                memset(pStrPath, 0, 256);
#if defined __linux__
                //SafeSprintf(link, sizeof(link), "/proc/self/exe", getpid());
                SafeSprintf(link, sizeof (link), "/proc/self/exe");

                int nCount = readlink(link, pStrPath, 256);
                if (nCount >= 265) {
                    TASSERT(false, "system path error");
                }
                //pStrPath[nCount] = 0;
                pStrPath = dirname(pStrPath);
#endif //__linux__
            }

            return pStrPath;
        }

        inline s64 GetTimeMillisecond() {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return tv.tv_sec * 1000 + tv.tv_usec / 1000;
        }

#ifdef __cplusplus
    }
#endif

    inline string IntAsString(const s32 value) {
        char str[128] = {0};
        SafeSprintf(str, sizeof(str), "%d", value);
        return string(str);
    }

    inline s32 StringAsInt(const char * value) {
        TASSERT(value, "null point");
        return atoi(value);
    }
}

#endif //__Tools_h__
