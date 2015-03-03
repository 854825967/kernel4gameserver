#ifndef __Kernel_h__
#define __Kernel_h__

#include "IKernel.h"
#include "ICore.h"

class Kernel : public tcore::IKernel, ICore {
public:
    static tcore::IKernel * getInstance();

    virtual bool Redry();
    virtual bool Initialize();
    virtual bool Destory();


    virtual bool StartTcpServer(tcore::ITcpServer * sever);
    virtual bool StartTcpClient(tcore::ITcpSocket * client);

    virtual void Shutdown();
    void Loop();

private:
    Kernel();
    ~Kernel();

private:
    bool m_bShutdown;
};

#endif //__Kernel_h__
