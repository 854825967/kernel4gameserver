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

void ClientConnectSick::Error(IKernel * pKernel, const s8 opt, void * pContext, const char * debug) {
    //ECHO_ERROR(debug);
}

s32 ClientConnectSick::Recv(IKernel * pKernel, const void * context, const s32 size) {
    //ECHO("size %d", size);
    //Send(context, size);
    return size;
}

void ClientConnectSick::Disconnect(IKernel * pKernel) {
    i --;
    ECHO_TRACE("connection Disconnect, connection count : %d", i);
    pKernel->KillTimer(this);
    delete this;
}

void ClientConnectSick::Connected(IKernel * pKernel) {
    i ++;
    ECHO("connection connected, connection count : %d", i);
    pKernel->StartTimer(100, this, 20000, 0);
}
