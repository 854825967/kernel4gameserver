/* 
 * File:   ClientMgr.cpp
 * Author: alax
 * 
 * Created on March 3, 2015, 10:33 AM
 */

#include "ClientMgr.h"
#include "Client.h"

bool ClientMgr::Initialize(IKernel * pKernel) {
    pKernel->StartTimer(1000000, this, 100, 0, 1000);

    return true;
}

bool ClientMgr::Launched(IKernel * pKernel) {

    return true;
}

bool ClientMgr::Destroy(IKernel * pKernel) {

    return true;
}


//will be called before first OnTimer, will not affect OnTimer count
void ClientMgr::OnStart(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {

}

void ClientMgr::OnTime(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {
    for (s32 i=0; i<2; i++) {
        Client * pClient = Client::Create();
        if (!pKernel->StartTcpClient(pClient, "10.240.58.22", 12580)) {
            pClient->Release();
        }
    }
}

//called after last OnTimer or remove timer, do not affect OnTimer count
//nonviolent will be false if timer is removed by developer,  otherwise true 
void ClientMgr::OnTerminate(IKernel * pKernel, const s32 sTimerID, bool nonviolent, s64 lTimetick) {

}
