/* 
 * File:   Gate.cpp
 * Author: alax
 * 
 * Created on March 3, 2015, 10:33 AM
 */

#include "Gate.h"
#include "IObjectMgr.h"

bool Gate::Initialize(IKernel * pKernel) {
    return true;
}

bool Gate::Launched(IKernel * pKernel) {
    pKernel->StartTcpServer(this, "0.0.0.0", 16666);

    pKernel->StartTcpServer(NEW ShutDownServer, "0.0.0.0", 12590);

    for (s32 i=0; i<512; i++) {
        ClientConnectSink * pSink = NEW ClientConnectSink;
        pKernel->StartTcpClient(pSink, "127.0.0.1", 16666);
    }

    return true;
}

bool Gate::Destroy(IKernel * pKernel) {
    return true;
}

void Gate::Error(IKernel * pKernel, ITcpSession * pSession) {

}

ITcpSession * Gate::MallocConnection(IKernel * pKernel) {
    return NEW ClientConnectSink;
}
