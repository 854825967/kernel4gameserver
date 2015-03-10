/* 
 * File:   ClientConnectSick.cpp
 * Author: alax
 * 
 * Created on March 3, 2015, 1:06 PM
 */

#include "ClientConnectSick.h"
#include "Tools.h"

ClientConnectSick::ClientConnectSick() {

}

void ClientConnectSick::Error(IKernel * pKernel, const s8 opt, const s32 code) {

}

s32 ClientConnectSick::Recv(IKernel * pKernel, void * context, const s32 size) {
    ECHO("%s", (const char *) context);
    return size;
}

void ClientConnectSick::Disconnect(IKernel * pKernel) {
    ECHO("connection breaked");
    //pKernel->KillTimer(this);
}

void ClientConnectSick::Connected(IKernel * pKernel) {
    ECHO("connection connected");
    
    for (s32 i = 0; i<500; i+=5) {
        Worker * p = NEW Worker(200);
        pKernel->StartTimer(i, p, 200, rand()%100, rand()%1000 + 2000);
        
        p = NEW Worker(2003);
        pKernel->StartTimer(i+1, p, 2003, rand()%100, rand()%1000 + 2000);
        
        p = NEW Worker(5000);
        pKernel->StartTimer(i+2, p, 5000, rand()%100, rand()%1000 + 2000);
        
        p = NEW Worker(1000);
        pKernel->StartTimer(i+3, p, 1000, rand()%100, rand()%1000 + 2000);
        
        p = NEW Worker(3000);
        pKernel->StartTimer(i+4, p, 3000, rand()%100, rand()%1000 + 2000);
    }
}


//called before first OnTimer, do not affect OnTimer count

void Worker::OnStart(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {

}

void Worker::OnTime(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {
    s64 lTick = tools::GetTimeMillisecond();
    if (lTick - lTimetick > 10) {
        ECHO("the tick of error : %ld, interval %ld", lTick - lTimetick, interval);
    }

    ECHO("worker~~~");
}

//called after last OnTimer or remove timer, do not affect OnTimer count
//nonviolent will be false if timer is removed by developer,  otherwise true 

void Worker::OnTerminate(IKernel * pKernel, const s32 sTimerID, bool nonviolent, s64 lTimetick) {

}
