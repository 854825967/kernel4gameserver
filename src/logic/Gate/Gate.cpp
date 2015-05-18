/* 
 * File:   Gate.cpp
 * Author: alax
 * 
 * Created on March 3, 2015, 10:33 AM
 */

#include "Gate.h"

bool Gate::Initialize(IKernel * pKernel) {
    return true;
}

bool Gate::Launched(IKernel * pKernel) {
    pKernel->StartTcpServer(this, "0.0.0.0", 16666);

    pKernel->StartTcpServer(NEW ShutDownServer, "0.0.0.0", 12590);

    pKernel->StartTimer(0, this, 100);

    return true;
}


void Gate::OnStart(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {

}

void Gate::OnTime(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {
    static s32 i = 0;
    s64 lTick = tools::GetTimeMillisecond();
    if (lTick - lTimetick > 30) {
        ECHO("error tick %ld", lTick - lTimetick);
    }

    ClientConnectSink * pSink = NEW ClientConnectSink;
    pKernel->StartTcpClient(pSink, "127.0.0.1", 16666);
}

void Gate::OnTerminate(IKernel * pKernel, const s32 sTimerID, bool nonviolent, s64 lTimetick) {

}

bool Gate::Destroy(IKernel * pKernel) {
    return true;
}

void Gate::Error(IKernel * pKernel, ITcpSession * pSession) {

}

ITcpSession * Gate::MallocConnection(IKernel * pKernel) {
    return NEW ClientConnectSink;
}
