#include "Kernel.h"
#include "TBundler.h"
using namespace tlib;

int main() {    
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);// ³õÊ¼»¯Windows Socket 2.2
#endif //WIN32
    Kernel * pKernel = (Kernel *) Kernel::getInstance();
    TASSERT(pKernel, "get kernel point error");
    bool res = pKernel->Initialize();
    TASSERT(res, "launch kernel error");

    pKernel->Loop();

    return 0;
}

