#include "Kernel.h"
#include "configmgr/Configmgr.h"
#include "epoller/epoller.h"
#include "logicmgr/Logicmgr.h"
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
    return Configmgr::getInstance() && Logicmgr::getInstance() && epoller::getInstance();
}

bool Kernel::Initialize() {
    return Configmgr::getInstance()->Initialize()
            && epoller::getInstance()->Initialize()
            && Logicmgr::getInstance()->Initialize();
}

bool Kernel::Destory() {
    Configmgr::getInstance()->Destory();
    Logicmgr::getInstance()->Destory();
    epoller::getInstance()->Destory();

    delete this;
    return true;
}

bool Kernel::StartTcpServer(tcore::ITcpServer * sever) {
    return epoller::getInstance()->AddServer(sever);
}

bool Kernel::StartTcpClient(tcore::ITcpSocket * client) {
    return epoller::getInstance()->AddClient(client);
}

void Kernel::Shutdown() {
    m_bShutdown = true;
}

void Kernel::Loop() {
    m_bShutdown = false;
    while (!m_bShutdown) {
        epoller::getInstance()->DonetIO(Configmgr::getInstance()->GetCoreConfig()->sNetframetick);
    }
}
