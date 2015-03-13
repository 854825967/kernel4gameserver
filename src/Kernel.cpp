#include "Kernel.h"
#include "configmgr/Configmgr.h"
#include "logicmgr/Logicmgr.h"
#include "timermgr/Timermgr.h"

#ifdef linux
#include "epoller/epoller.h"
typedef epoller netengine;
#endif //linux

#ifdef WIN32
#endif //WIN32
using namespace tcore;

Kernel::Kernel() {
    m_bShutdown = true;
}

Kernel::~Kernel() {

}

IKernel * Kernel::getInstance() {
    static Kernel * p = NULL;
    if (!p) {
        p = NEW Kernel;
        if (!p->Redry()) {
            delete p;
            p = NULL;
        }
    }

    return p;
}

bool Kernel::Redry() {
    return Configmgr::getInstance()
        && Timermgr::getInstance()
        //&& netengine::getInstance() 
        && Logicmgr::getInstance();
}

bool Kernel::Initialize() {
    return Configmgr::getInstance()->Initialize()
            && Timermgr::getInstance()->Initialize()
            //&& netengine::getInstance()->Initialize()
            && Logicmgr::getInstance()->Initialize();
}

bool Kernel::Destory() {
    Configmgr::getInstance()->Destory();
    Logicmgr::getInstance()->Destory();
    Timermgr::getInstance()->Destory();
    //netengine::getInstance()->Destory();

    delete this;
    return true;
}

void Kernel::Shutdown() {
    m_bShutdown = true;
}

void Kernel::Loop() {
    m_bShutdown = false;
    while (!m_bShutdown) {
        // netengine::getInstance()->DonetIO(Configmgr::getInstance()->GetCoreConfig()->sNetframetick);
        s64 tick = Timermgr::getInstance()->Dotimer();
        if (tick > 1) {
            ECHO("tick :%ld", tick);
        }
    }
}

bool Kernel::StartTcpServer(tcore::ITcpServer * sever) {
    //return netengine::getInstance()->AddServer(sever);
    return false;
}

bool Kernel::StartTcpClient(tcore::ITcpSocket * client) {
    //return netengine::getInstance()->AddClient(client);
    return false;
}

// tiemr interface 
bool Kernel::StartTimer(s32 id, tcore::ITimer * timer, s64 interval, s64 delay, s64 loop) {
    return Timermgr::getInstance()->StartTimer(id, timer, interval, delay, loop);
}

bool Kernel::KillTimer(s32 id, tcore::ITimer * timer) {
    return Timermgr::getInstance()->KillTimer(id, timer);
}

bool Kernel::KillTimer(tcore::ITimer * timer) {
    return Timermgr::getInstance()->KillTimer(timer);
}

