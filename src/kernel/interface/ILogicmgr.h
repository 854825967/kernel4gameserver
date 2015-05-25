#ifndef __ILogicmgr_h__
#define __ILogicmgr_h__

#include "ICore.h"
#include "IKernel.h"
#include "IModule.h"

class ILogicmgr : public ICore {
public:
    virtual IModule * FindModule(const char * pModuleName) = 0;
};

#endif //__ILogicmgr_h__
