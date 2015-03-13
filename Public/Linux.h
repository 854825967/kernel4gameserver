#ifndef __Linux_h__
#define __Linux_h__

#ifdef linux
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/time.h>
#include <libgen.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef char s8;
typedef short s16;
typedef int32_t s32;
typedef int64_t s64;

#define THREAD_FUN void*
#define ThreadID pthread_id

#define CSLEEP(n) usleep(n)
#define SafeSprintf snprintf

#define ECHO(format, a...) { \
    char log[4096] = {0}; \
    SafeSprintf(log, 4096, "%s:%d:%s"#format, __FILE__, __LINE__, __FUNCTION__, ##a); \
    printf("%s\n", log);}

#endif //linux

#endif //__Linux_h__
