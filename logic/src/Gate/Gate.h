/* 
 * File:   gate.h
 * Author: alax
 *
 * Created on March 3, 2015, 10:33 AM
 */

#ifndef __Gate_h__
#define	__Gate_h__

#include "IGate.h"
#include "IKernel.h"

class Gate : public IGate, ITcpServer, ITimer {
public:
    virtual bool Initialize(IKernel * pKernel);
    virtual bool Launched(IKernel * pKernel);
    virtual bool Destroy(IKernel * pKernel);
    virtual void Error(IKernel * pKernel, const s8 opt, void * pContext, const char * debug);
    virtual ITcpSocket * MallocConnection(IKernel * pKernel);

    virtual void OnStart(IKernel * pKernel, const s32 sTimerID, s64 lTimetick);
    virtual void OnTime(IKernel * pKernel, const s32 sTimerID, s64 lTimetick);

    //called after last OnTimer or remove timer, do not affect OnTimer count
    //nonviolent will be false if timer is removed by developer,  otherwise true 
    virtual void OnTerminate(IKernel * pKernel, const s32 sTimerID, bool nonviolent, s64 lTimetick);

};

#endif	/* GATE_H */

