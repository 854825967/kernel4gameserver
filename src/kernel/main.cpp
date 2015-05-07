#include "Kernel.h"
#include "Tools.h"
#include "CDumper.h"


class Dumper {
public:
    Dumper() {
        tlib::CDumper::GetInstance().SetDumpFileName((tools::GetCurrentTimeString() + ".dump").c_str());
    }
};

Dumper dumper;

int main() {    
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);// ³õÊ¼»¯Windows Socket 2.2
#endif //WIN32
    Kernel * pKernel = (Kernel *) Kernel::getInstance();
    TASSERT(pKernel, "get kernel point error");
    bool res = pKernel->Initialize();
    TASSERT(res, "launch kernel error");

    pKernel->Loop();

    pKernel->Destory();

    TRACEMEM;

    delete tools::GetAppPath();
    return 0;
}

