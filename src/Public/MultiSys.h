#ifndef __MultiSys_h__
#define __MultiSys_h__

#include "Winsys.h"
#include "Linux.h"

#ifdef __cplusplus
extern "C" {
#endif 
    void _AssertionFail(const char * file, int line, const char * funname, const char * debug);
#ifdef __cplusplus
};
#endif

//为了找到内存泄露的元凶，我要记录下每一处new所在的文件名和所在行。于是再次重载了operator new：
// void * operator new(size_t size, const char* file, const size_t line);
// void * operator new[](size_t size, const char* file, const size_t line);
// //为了避免编译时出现warning C4291(没有与operator new(unsigned int,const char *,const unsigned int) 匹配的delete)，又重载了
// void operator delete (void * pointer);
// void operator delete[](void * pointer);
//void TraceMemory();

#if defined _DEBUG
/***debug modle***/

#define NEW new //DEBUG_CLIENTBLOCK
#define TRACEMEM //TraceMemory();

#ifdef WIN32
#define TASSERT(p, format, ...) { \
    char debug[4096] = {0}; \
    SafeSprintf(debug, sizeof(debug), format, ##__VA_ARGS__); \
    ((p) ? (void)0 : (void)_AssertionFail(__FILE__, __LINE__, __FUNCTION__, debug)); \
}
#else
#define TASSERT(p, format, a...) { \
    char debug[4096] = {0}; \
    SafeSprintf(debug, sizeof(debug), format, ##a); \
    ((p) ? (void)0 : (void)_AssertionFail(__FILE__, __LINE__, __FUNCTION__, debug)); \
}
#endif //WIN32

#else
/***no debug***/

#define NEW new
#define TRACEMEM

#define TASSERT(p, format, ...)
#endif //defined _DEBUG

#endif //define __MultiSys_h__

