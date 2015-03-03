#include "MultiSys.h"
#include "TStream.h"
#include "epoller.h"
#include "IKernel.h"
#include "CRand.h"
#include "TQueue.h"
#include "Tools.h"
#include "configmgr/Configmgr.h"
#include "logicmgr/Logicmgr.h"
#include "Kernel.h"
#include <fcntl.h>
#include <netinet/in.h>

using namespace tlib;

int main() {
    Kernel * pKernel = (Kernel *)Kernel::getInstance();
    TASSERT(pKernel, "get kernel point error");
    bool res = pKernel->Initialize();
    TASSERT(res, "launch kernel error");
    
    pKernel->Loop();
   
    return 0;
}

