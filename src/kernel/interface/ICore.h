#ifndef __ICore_h__
#define __ICore_h__

#include "MultiSys.h"

class ICore {
public:
    virtual ~ICore() {}

    virtual bool Redry() = 0;
    virtual bool Initialize() = 0;
    virtual bool Destory() = 0;
};

#endif //__ICore_h__
