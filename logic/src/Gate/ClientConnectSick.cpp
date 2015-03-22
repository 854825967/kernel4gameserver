/* 
 * File:   ClientConnectSick.cpp
 * Author: alax
 * 
 * Created on March 3, 2015, 1:06 PM
 */

#include "ClientConnectSick.h"
#include "Tools.h"

static s32 i = 0;

ClientConnectSick::ClientConnectSick() {

}

ClientConnectSick::~ClientConnectSick() {

}

void ClientConnectSick::OnStart(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {

}

//called before first OnTimer, do not affect OnTimer count
void ClientConnectSick::OnTime(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {
    Close();
}

//called after last OnTimer or remove timer, do not affect OnTimer count
//nonviolent will be false if timer is removed by developer,  otherwise true 
void ClientConnectSick::OnTerminate(IKernel * pKernel, const s32 sTimerID, bool nonviolent, s64 lTimetick) {

}

void ClientConnectSick::Error(IKernel * pKernel, const s8 opt, void * pContext, const char * debug) {
    ECHO("opt %d, error %s", opt, debug);
}

s32 ClientConnectSick::Recv(IKernel * pKernel, const void * context, const s32 size) {
    //ECHO("size %d", size);
    //Send(context, size);
    return size;
}

void ClientConnectSick::Disconnect(IKernel * pKernel) {
    i --;
    ECHO("connection Disconnect, connection count : %d", i);
    pKernel->KillTimer(this);
    delete this;
}

void ClientConnectSick::Connected(IKernel * pKernel) {
    i ++;
    ECHO("connection connected, connection count : %d", i);
    pKernel->StartTimer(100, this, 2000, 0);
}
