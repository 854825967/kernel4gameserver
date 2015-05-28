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

bool Gate::Destroy(IKernel * pKernel) {
    return true;
}

void Gate::Error(IKernel * pKernel, ITcpSession * pSession) {
    ECHO("%lx", pSession);
}

ITcpSession * Gate::MallocConnection(IKernel * pKernel) {
    return ClientConnectSink::Create();
}
