#ifndef __IConfigmgr_h__
#define __IConfigmgr_h__

#include "MultiSys.h"
#include "ICore.h"

#include <vector>
using namespace std;

struct sCoreConfig {
    //server logic main loop frame tick
    s32 sLoopduration; //millisecond
    
    //socket module
    s32 sNetThdCount; //net work thread count
    s32 sNetframetick; //net logic loop frame tick
    
    //http module
    s32 sHttpThdCount; //http work thread count
    s32 sHttpframetick; //http logic loop frame tick
    
    //timer module
    s32 sTimerlooptick;
};

struct sModuleConfig {
    string strModulePath;
    vector<string> vctModules;
};

class IConfigmgr : public ICore {
public:
    virtual const sCoreConfig * GetCoreConfig() = 0;
    virtual const sModuleConfig * GetModuleConfig() = 0;
    
};

#endif //__IConfigmgr_h__
