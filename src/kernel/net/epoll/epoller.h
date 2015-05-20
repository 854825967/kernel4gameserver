#ifndef __epoller_h__
#define __epoller_h__
#include "INetengine.h"
#include "epollheader.h"
#include "epollWorker.h"

class epoller : public INetengine {
public:
    static INetengine * getInstance();
    
    bool Redry();
    bool Initialize();
    bool Destory();

    bool AddServer(tcore::ITcpServer * server, const char * ip, const s32 port);
    bool AddClient(tcore::ITcpSession * client, const char * ip, const s32 port);

    epollWorker * BalancingWorker();
    
//    inline void remove_handler(s64 handler) {
//        epoll_ctl(m_epollfd, EPOLL_CTL_DEL, handler, NULL);
//    }
    
    s64 Processing(s64 overtime);
private:
    epoller();
    ~epoller();
    
    s64 DealEvent(s64 lTick);
    void DealAccept();
    void DealConnect();

private:
    s64 m_lEpollFD;
    EPOLLEREVENT_PO0L m_oEpeventPool;
    
    s32 m_nWorkerCount;
    epollWorker * m_pWorkerAry;
};

#endif //__epoller_h__

