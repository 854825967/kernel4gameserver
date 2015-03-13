#ifndef __Windows_h__
#define __Windows_h__

#ifdef WIN32
#include <stdio.h>
#ifndef _WINSOCK2API_  
#include <WinSock2.h>  
#endif  
#ifndef _WINDOWS_  
#include <Windows.h>  
#endif  
#include <shlwapi.h>
#include <Mswsock.h>
#include <time.h>
#include <assert.h>
#include <crtdbg.h>
#include <process.h>
#include <tchar.h>
#include <stdint.h>
#include <stdlib.h>
#include <direct.h>

#pragma comment( lib, "ws2_32.lib" )
#pragma comment(lib, "shlwapi.lib")

typedef unsigned char u8;
typedef unsigned short u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef char s8;
typedef short s16;
typedef int32_t s32;
typedef int64_t s64;

#define THREAD_FUN unsigned int
#define ThreadID pthread_id

#define CSLEEP(n) usleep(n)
#define SafeSprintf sprintf_s

#define ECHO(format, ...) { \
    char log[4096] = {0}; \
    SafeSprintf(log, 4096, "%s:%d:%s"#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    printf("%s\n", log);}

#endif //WIN32

#endif //__Windows_h__

