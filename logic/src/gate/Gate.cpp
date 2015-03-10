/* 
 * File:   Gate.cpp
 * Author: alax
 * 
 * Created on March 3, 2015, 10:33 AM
 */

#include "Gate.h"
#include "ClientConnectSick.h"

bool Gate::Initialize(IKernel * pKernel) {
    ECHO("hello, I'm gate");
    return true;
}

bool Gate::Launched(IKernel * pKernel) {
    ECHO("ready go, I'm gate");

    InitAddr("0.0.0.0", 12580);
    pKernel->StartTcpServer(this);

    return true;
}

bool Gate::Destroy(IKernel * pKernel) {
    ECHO("good bye, I'm gate");
    return true;
}

void Gate::Error(IKernel * pKernel, const s8 opt, const s32 code) {

}

ITcpSocket * Gate::MallocConnection(IKernel * pKernel) {
    ECHO("NEW CONNECTION");

    return NEW ClientConnectSick;
}
