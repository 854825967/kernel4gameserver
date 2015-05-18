#ifndef __ILogger_h__
#define __ILogger_h__

#include "ICore.h"

class ILogger : public ICore {
public:
    virtual ~ILogger() {}
    virtual void Log(const char * log) = 0;
};

#endif //__ICore_h__
