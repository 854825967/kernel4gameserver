#ifndef __epoller_h__
#define __epoller_h__
#include "IKernel.h"
#include "epoller_header.h"
#include "INetengine.h"

class epoller_worker;

class epoller : public INetengine {
public:
    static INetengine * getInstance();
    
    bool Redry();
    bool Initialize();
    bool Destory();

    bool AddServer(tcore::ITcpServer * server);
    bool AddClient(tcore::ITcpSocket * client);
    s64 DonetIO(s64 overtime);

    epoller_worker * BalancingWorker();
    
    inline void remove_handler(s64 handler) {
        epoll_ctl(m_epollfd, EPOLL_CTL_DEL, handler, NULL);
    }
    
private:
    epoller();
    ~epoller();
    
    s64 DealEvent(s64 lTick);
    void DealAccept();
    void DealConnect();

private:
    s64 m_epollfd;
    epoller_worker * m_pWorkerAry;
    s32 m_nWorkerCount;
};

#endif //__epoller_h__

