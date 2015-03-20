#ifndef __iocphead_h__
#define __iocphead_h__

#include "INetengine.h"
#include "MultiSys.h"
#include "TPool.h"
#include "TQueue.h"
#include "configmgr/Configmgr.h"
using namespace tlib;
using namespace tcore;

struct iocp_event {
    iocp_event() {
        buffLen = Configmgr::getInstance()->GetCoreConfig()->sNetRecvSize;
        pBuff = NEW char[buffLen];
        TASSERT(pBuff, "wtf");
    }

    void clear() {
        char * buff = pBuff;
        s32 len = buffLen;
        memset(this, 0, sizeof(*this));
        pBuff = buff;
        buffLen = len;
    }

    ~iocp_event() {
        delete[] pBuff;
    }

    OVERLAPPED ol;  /** 不解释,该成员变量必须放在第一位 */
    s8 opt;   /** 事件类型 */
    s32 code; /** 错误码 */
    WSABUF wbuf; /** 读写缓冲区结构体变量 */
    DWORD dwBytes, dwFlags; /** 一些在读写时用到的标志性变量 */
    char * pBuff;; /** 自己的缓冲区 */
    s32 buffLen;
    s64 socket; /** 句柄 **/
    struct sockaddr_in remote; /** 远端地址 **/
    tcore::ISocket * pContext; /** 上下文指针 */
};

extern tlib::TPool<iocp_event, true> g_poolIocpevent;

// only for debug
extern CLockUnit g_lock;

// async connect and accept api address
extern LPFN_ACCEPTEX g_pFunAcceptEx;
extern LPFN_CONNECTEX g_pFunConnectEx;

LPFN_ACCEPTEX GetAcceptExFun();
LPFN_CONNECTEX GetConnectExFun();
void formartIocpevent(struct iocp_event * & pEvent, ISocket * p, const s64 socket, const s8 event);

#endif //__iocphead_h__
