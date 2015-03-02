#ifndef __MultiSys_h__
#define __MultiSys_h__

#include "Linux.h"

#ifdef __cplusplus
extern "C" {
#endif 
    void _AssertionFail(const char * file, int line, const char * funname, const char * debug);
#ifdef __cplusplus
};
#endif

#ifdef _DEBUG
/***debug modle***/
#define NEW new
#define TASSERT(p, format, a...) { \
    char debug[4096] = {0}; \
    SafeSprintf(debug, sizeof(debug), format, ##a); \
    ((p) ? (void)0 : (void)_AssertionFail(__FILE__, __LINE__, __FUNCTION__, debug)); \
}

#else
/***no debug***/
#define NEW new
#define TASSERT(p, debug)
#endif //defined _DEBUG

#endif //define __MultiSys_h__

