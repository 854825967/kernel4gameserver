#ifndef __iocper_h__
#define __iocper_h__

#include "INetengine.h"
#include "TPool.h"
#include "TQueue.h"
#include "CThread.h"

enum {
    IO_TYPE_RECV = 0,
    IO_TYPE_SEND
};

struct iocp_event {
    OVERLAPPED ol;  /** 不解释,该成员变量必须放在第一位 */
    s8 event;   /** 事件类型 */
    s8 iotype; /** io类型, send还是recve */
    s32 nerron; /** 错误码 */
    WSABUF wbuf; /** 读写缓冲区结构体变量 */
    DWORD dwBytes, dwFlags; /** 一些在读写时用到的标志性变量 */
    s32 ioBytes;
    char buff[BUFF_SIZE]; /** 自己的缓冲区 */
    s64 socket; /** 句柄 **/
    struct sockaddr_in remote; /** 远端地址 **/
    tcore::ISocket * p; /** 上下文指针 */
};

class iocper : public INetengine, public tlib::CThread {
public:
    static INetengine * getInstance();

    virtual bool Redry();
    virtual bool Initialize();
    virtual bool Destory();

    virtual bool AddServer(tcore::ITcpServer * server);
    virtual bool AddClient(tcore::ITcpSocket * client);
    virtual s64 DonetIO(s64 overtime);
    virtual void Run();

private:
    iocper() {
        m_hCompletionPort = NULL;
        m_bShutdown = true;
        m_nThreadCount = 0;
    }

private:
    HANDLE m_hCompletionPort;
    bool m_bShutdown;
    s32 m_nThreadCount;

    tlib::TPool<iocp_event, true> m_poolIocpevent;
    tlib::TQueue<iocp_event *, true, 4096> m_queueIocpevent;
    tlib::CLockUnit m_threadLock;

    // async connect and accept api address
    static LPFN_ACCEPTEX s_pFunAcceptEx;
    static LPFN_CONNECTEX s_pFunConnectEx;
};


#endif //__iocper_h__
