#ifndef __ClientMgr_h__
#define __ClientMgr_h__

#include "IModule.h"
using namespace tcore;

class ClientMgr : public IModule, ITimer {
public:
    virtual bool Initialize(IKernel * pKernel);
    virtual bool Launched(IKernel * pKernel);
    virtual bool Destroy(IKernel * pKernel);

    //will be called before first OnTimer, will not affect OnTimer count
    virtual void OnStart(IKernel * pKernel, const s32 sTimerID, s64 lTimetick); 
    virtual void OnTime(IKernel * pKernel, const s32 sTimerID, s64 lTimetick);
    //called after last OnTimer or remove timer, do not affect OnTimer count
    //nonviolent will be false if timer is removed by developer,  otherwise true 
    virtual void OnTerminate(IKernel * pKernel, const s32 sTimerID, bool nonviolent, s64 lTimetick);
private:

};

#endif //__ClientMgr_h__
