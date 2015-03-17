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

void ClientConnectSick::Error(IKernel * pKernel, const s8 opt, const s32 code) {
    //ECHO("opt %d, error %d", opt, code);
}

s32 ClientConnectSick::Recv(IKernel * pKernel, void * context, const s32 size) {
    //ECHO("%s", (const char *) context);
    Close();
    return size;
}

void ClientConnectSick::Disconnect(IKernel * pKernel) {
    i --;
    //ECHO("connection Disconnect, connection count : %d", i);
}

void ClientConnectSick::Connected(IKernel * pKernel) {
    i ++;
    //ECHO("connection connected, connection count : %d", i);
}
