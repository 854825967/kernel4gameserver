/* 
 * File:   ClientConnectSick.h
 * Author: alax
 *
 * Created on March 3, 2015, 1:06 PM
 */

#ifndef __ClientConnectSick_h_
#define	__ClientConnectSick_h_

#include "IKernel.h"
using namespace tcore;


class ClientConnectSick : public ITcpSocket, public ITimer {
public:
    ClientConnectSick();
    ~ClientConnectSick();

    virtual void Error(IKernel * pKernel, const s8 opt, void * pContext, const char * debug);
    virtual s32 Recv(IKernel * pKernel, const void * context, const s32 size);

    virtual void Disconnect(IKernel * pKernel);
    virtual void Connected(IKernel * pKernel);

    virtual void OnStart(IKernel * pKernel, const s32 sTimerID, s64 lTimetick);
    
    //called before first OnTimer, do not affect OnTimer count
    virtual void OnTime(IKernel * pKernel, const s32 sTimerID, s64 lTimetick);

    //called after last OnTimer or remove timer, do not affect OnTimer count
    //nonviolent will be false if timer is removed by developer,  otherwise true 
    virtual void OnTerminate(IKernel * pKernel, const s32 sTimerID, bool nonviolent, s64 lTimetick);

private:

};

#endif	/* __ClientConnectSick_h_ */

