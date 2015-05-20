#ifndef __iocphead_h__
#define __iocphead_h__

#include "INetengine.h"
#include "MultiSys.h"
#include "TQueue.h"
#include "CPipe.h"
#include "TPool.h"
#include "configmgr/Configmgr.h"
using namespace tlib;
using namespace tcore;

struct iocp_event {
    explicit iocp_event(s32 len) {
        TASSERT(len >= 0, "wtf");
        buffLen = len;
        (len > 0)?(pBuff = NEW char[len]):(pBuff = NULL);
        clear();
    }

    void clear() {
        char * buff = pBuff;
        s32 len = buffLen;
        memset(this, 0, sizeof(*this));
        pBuff = buff;
        buffLen = len;
        wbuf.buf = pBuff;
        wbuf.len = buffLen;
    }

    ~iocp_event() {
        if (pBuff) {
            delete[] pBuff;
        }
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
    void * pContext; /** 上下文指针 */
};

extern tlib::TPool<CPipe, true> g_oCPipePool;


// async connect and accept api address
extern LPFN_ACCEPTEX g_pFunAcceptEx;
extern LPFN_CONNECTEX g_pFunConnectEx;

LPFN_ACCEPTEX GetAcceptExFun();
LPFN_CONNECTEX GetConnectExFun();
void formartIocpevent(struct iocp_event * & pEvent, void * p, const s64 socket, const s8 event);

#endif //__iocphead_h__
