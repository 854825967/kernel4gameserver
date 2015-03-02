#include "MultiSys.h"
#include "TStream.h"
#include "epoller.h"
#include "IKernel.h"
#include "CRand.h"
#include "TQueue.h"
#include "Tools.h"
#include "configmgr/Configmgr.h"
#include <fcntl.h>
#include <netinet/in.h>

using namespace tlib;

int main() {

    IConfigmgr * p = Configmgr::getInstance();
    
//    CRand rand;
//    rand.SetSeed(100);
//    
//    while (true) {
//        printf("%s : Rand : %d\n", tools::GetAppPath(), rand.Rand());
//        
//        CSLEEP(1000000);
//    }
    
//    TQueue<int, false, 512> queue;
//
//    epoller ep;
//    ep.Initialize();
// 
//    TSocket * pS = NEW TSocket;
//    pS->Initialize("0.0.0.0", 12899);
//    ep.AddServer(pS);
//
//    IConsole * pConsole = NEW IConsole;
//    pConsole->Initialize("0.0.0.0", 12580);
//    ep.AddServer(pConsole);
//    
//    Test clients1;
//    clients1.p = &ep;
//    clients1.Start(2);
//    
//    while (true) {
//        ep.DonetIO(100);
//    }

    return 0;
}

