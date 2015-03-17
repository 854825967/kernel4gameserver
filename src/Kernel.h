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

    // tiemr interface 
    virtual bool StartTimer(s32 id, tcore::ITimer * timer, s64 interval, s64 delay, s64 loop) ;
    virtual bool KillTimer(s32 id, tcore::ITimer * timer);
    virtual bool KillTimer(tcore::ITimer * timer);

    virtual void Shutdown();
    void Loop();

private:
    Kernel();
    ~Kernel();

private:
    bool m_bShutdown;
};

#endif //__Kernel_h__
