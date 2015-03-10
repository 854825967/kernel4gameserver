#include "Kernel.h"
using namespace tlib;

#include <list>
using namespace std;

int main() {    
    Kernel * pKernel = (Kernel *) Kernel::getInstance();
    TASSERT(pKernel, "get kernel point error");
    bool res = pKernel->Initialize();
    TASSERT(res, "launch kernel error");

    pKernel->Loop();

    return 0;
}

