#include "Kernel.h"
#include "configmgr/Configmgr.h"
#include "logicmgr/Logicmgr.h"
#include "timermgr/Timermgr.h"
#include "log/Logger.h"
using namespace tcore;

#ifdef linux
#include "epoller/epoller.h"
typedef epoller netengine;
#endif //linux

#ifdef WIN32
#include "net/iocp/iocper.h"
typedef iocper netengine;
#endif //WIN32

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
        && netengine::getInstance() 
        && Logicmgr::getInstance()
        && Logger::getInstance();
}

bool Kernel::Initialize() {
    return Configmgr::getInstance()->Initialize()
            && Timermgr::getInstance()->Initialize()
            && netengine::getInstance()->Initialize()
            && Logicmgr::getInstance()->Initialize()
            && Logger::getInstance()->Initialize();
}

bool Kernel::Destory() {
    Configmgr::getInstance()->Destory();
    Logicmgr::getInstance()->Destory();
    Timermgr::getInstance()->Destory();
    netengine::getInstance()->Destory();
    Logger::getInstance()->Destory();

    delete this;
    return true;
}

void Kernel::Shutdown() {
    m_bShutdown = true;
}

void Kernel::Loop() {
    m_bShutdown = false;
    while (!m_bShutdown) {
        s64 lUse = netengine::getInstance()->Processing(Configmgr::getInstance()->GetCoreConfig()->sNetframetick);
        s64 tick = Timermgr::getInstance()->Dotimer();
    }
}

IModule * Kernel::FindModule(const char * name) {
	return Logicmgr::getInstance()->FindModule(name);
}

bool Kernel::StartTcpServer(tcore::ITcpServer * server, const char * ip, const s32 port) {
    return netengine::getInstance()->AddServer(server, ip, port);
}

bool Kernel::StartTcpClient(tcore::ITcpSession * client, const char * ip, const s32 port) {
    return netengine::getInstance()->AddClient(client, ip, port);
}


void Kernel::LogDebug(const char * debug) {
    char szBuff[4096] = {0};
    SafeSprintf(szBuff, sizeof(szBuff), "debug | %s | %s\n", tools::GetCurrentTimeString().c_str(), debug);
    Logger::getInstance()->Log(szBuff);
}

void Kernel::LogTrace(const char * debug) {
    char szBuff[4096] = {0};
    SafeSprintf(szBuff, sizeof(szBuff), "trace | %s | %s\n", tools::GetCurrentTimeString().c_str(), debug);
    Logger::getInstance()->Log(szBuff);
}

void Kernel::LogError(const char * debug) {
    char szBuff[4096] = {0};
    SafeSprintf(szBuff, sizeof(szBuff), "error | %s | %s\n", tools::GetCurrentTimeString().c_str(), debug);
    Logger::getInstance()->Log(szBuff);
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

