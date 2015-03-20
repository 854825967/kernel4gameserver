/* 
 * File:   Gate.cpp
 * Author: alax
 * 
 * Created on March 3, 2015, 10:33 AM
 */

#include "Gate.h"
#include "IObjectMgr.h"
#include "ClientConnectSick.h"

bool Gate::Initialize(IKernel * pKernel) {
    return true;
}

bool Gate::Launched(IKernel * pKernel) {

	InitAddr("0.0.0.0", 12580);
	pKernel->StartTcpServer(this);

    return true;
}

bool Gate::Destroy(IKernel * pKernel) {
    return true;
}

void Gate::Error(IKernel * pKernel, const s8 opt, void * pContext, const char * debug) {
    ECHO(debug);
}

ITcpSocket * Gate::MallocConnection(IKernel * pKernel) {
    static s32 i = 1;
    //ECHO("NEW CONNECTION %d", i++);
    return NEW ClientConnectSick;
}
