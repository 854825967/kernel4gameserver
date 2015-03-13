#include "MultiSys.h"
#include <assert.h>
#ifdef __cplusplus
extern "C" {
#endif
void _AssertionFail(const char * strFile, int nLine, const char * pFunName, const char * debug) {
    fflush(stdout);
    fprintf(stderr, "\nAsssertion failed\n=======assert string=======\nfile:%s\nline:%d\nfunction:%s\ndebug:%s\n===========================\n", strFile, nLine, pFunName, debug);
    fflush(stderr);
    assert(false);
}

#ifdef __cplusplus
}
#endif

