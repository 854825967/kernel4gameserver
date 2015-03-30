/* 
 * File:   Gate.cpp
 * Author: alax
 * 
 * Created on March 3, 2015, 10:33 AM
 */

#include "Gate.h"
#include "IObjectMgr.h"
#include "ClientConnectSink.h"

bool Gate::Initialize(IKernel * pKernel) {
    return true;
}

bool Gate::Launched(IKernel * pKernel) {
    InitAddr("0.0.0.0", 12580);
    pKernel->StartTcpServer(this);

    pKernel->StartTimer(1000, this, 2000);

    return true;
}

bool Gate::Destroy(IKernel * pKernel) {
    return true;
}

void Gate::Error(IKernel * pKernel, const s8 opt, void * pContext, const char * debug) {

}

void Gate::OnStart(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {

}

void Gate::OnTime(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {

}

//called after last OnTimer or remove timer, do not affect OnTimer count
//nonviolent will be false if timer is removed by developer,  otherwise true 
void Gate::OnTerminate(IKernel * pKernel, const s32 sTimerID, bool nonviolent, s64 lTimetick) {

}

ITcpSocket * Gate::MallocConnection(IKernel * pKernel) {
    return NEW ClientConnectSink;
}
