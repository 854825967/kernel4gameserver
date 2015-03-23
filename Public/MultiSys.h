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

#if defined _DEBUG
/***debug modle***/
#define NEW new

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
#define TASSERT(p, format, a...)
#endif //defined _DEBUG

#endif //define __MultiSys_h__

