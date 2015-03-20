#ifndef __Tools_h__
#define __Tools_h__

#include "MultiSys.h"
#include <string>
using namespace std;

namespace tools {
#ifdef __cplusplus
    extern "C" {
#endif

        inline ThreadID GetCurrentThreadID() {
#ifdef _WIN32
            return ::GetCurrentThreadId();
#endif //_WIN32
        }

        inline const char * GetAppPath() {
            static char * pStrPath = NULL;

            if (NULL == pStrPath) {
                pStrPath = NEW char[256];
                char link[256];
                memset(link, 0, 256);
                memset(pStrPath, 0, 256);
#ifdef linux
                //SafeSprintf(link, sizeof(link), "/proc/self/exe", getpid());
                SafeSprintf(link, sizeof (link), "/proc/self/exe");

                int nCount = readlink(link, pStrPath, 256);
                if (nCount >= 265) {
                    TASSERT(false, "system path error");
                }
                //pStrPath[nCount] = 0;
                pStrPath = dirname(pStrPath);
#endif //linux

#ifdef WIN32
                GetModuleFileName(NULL, pStrPath, 256);
                PathRemoveFileSpec(pStrPath);
#endif //WIN32
            }

            return pStrPath;
        }

        inline s64 GetTimeMillisecond() {
#ifdef linux
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return tv.tv_sec * 1000 + tv.tv_usec/1000;
#endif //linux

#ifdef WIN32    
            SYSTEMTIME wtm;
            GetLocalTime(&wtm);
            struct tm tTm;
            tTm.tm_year     = wtm.wYear - 1900;
            tTm.tm_mon      = wtm.wMonth;
            tTm.tm_mday     = wtm.wDay;
            tTm.tm_hour     = wtm.wHour;
            tTm.tm_min      = wtm.wMinute;
            tTm.tm_sec      = wtm.wSecond;
            tTm.tm_isdst    = -1;
            return (s64)mktime(&tTm) * 1000 + (s64)wtm.wMilliseconds;
#endif //WIN32
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
