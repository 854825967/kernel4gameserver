#ifndef __epoller_h__
#define __epoller_h__
#include <sys/epoll.h>
#include "TQueue.h"
#include "IKernel.h"
#include "header.h"
#include "epoll_waiter.h"
#include "INetengine.h"
#include "tinyxml/tinyxml.h"

class epoll_worker;
class epoll_waiter;

class epoller : public INetengine {
public:
    static INetengine * getInstance();
    
    bool Redry();
    bool Initialize();
    bool Destory();

    void Run();

    bool AddServer(tcore::ITcpServer * server);
    bool AddClient(tcore::ITcpSocket * client);
    s64 DonetIO(s64 overtime);

    inline s32 GetEpollDesc() {
        return m_epoll;
    }

    inline bool epoller_CTL(s32 model, s32 handler, struct epoll_event * pev) {
        s32 res = epoll_ctl(m_epoll, model, handler, pev);
        TASSERT(res == 0, strerror(errno));
        return res == 0;
    }

    void AddEvent(struct epoller_data * data);
    void AddIO(struct epoller_data * data);

private:
    epoller();
    ~epoller();
    
    
    bool InitEpollHandler(s32 & handler, s32 count);
    void Doevent();

private:
    s32 m_epoll;
    TQueue<struct epoller_data *, true, EPOLLER_DATA_COUNT> m_oIOQueue;
    epoll_worker * m_pWorkerAry;
    epoll_waiter * m_pWaiter;
    s32 m_nWorkerCount;
};

#endif //__epoller_h__

