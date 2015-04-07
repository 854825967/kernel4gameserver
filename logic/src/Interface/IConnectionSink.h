/* 
 * File:   IConnectionSink.h
 * Author: alax
 *
 * Created on March 3, 2015, 1:06 PM
 */

#ifndef __IConnectionSink_h_
#define	__IConnectionSink_h_

#include "IKernel.h"
#include "TBundler.h"

#define RGS_MSG_CALL(msgid, address) RgsMsgCall(msgid, (memfun)&address, #address)
#define UNRGS_MSG_CALL(msgid, address) UnRgsMsgCall(msgid, address)


class IConnectionSink : public tcore::ITcpSocket {
public:
    typedef void (IConnectionSink::*memfun)(tcore::IKernel *, const s32, const void *, const s32);

    virtual s32 Recv(tcore::IKernel * pKernel, const void * context, const s32 size) {        
        s32 nSize = 0;
        if (*(const s32 *)context <= size && size > 8) {
            nSize= *(const s32 *)context;
            s32 nMsgID = *((const s32 *)((const char *)context + 4));
            ECHO("MSG id : %d, size %d", nMsgID, nSize);
            m_bundlerpool.call_bundler(nMsgID, pKernel, nMsgID, (const char *)context + 8, nSize - 8);
        }

        return nSize;
    }

    void SendMSG(const s32 & nMsgID, const void * pContext, const s32 nSize) {
        s32 nLen = sizeof(s32) + sizeof(s32) + nSize;
        Send(&nLen, sizeof(s32));
        Send(&nMsgID, sizeof(s32));
        Send(pContext, nSize);
    }

    bool RgsMsgCall(const s32 nMsgID, memfun address, const char * strdebug) {
        return m_bundlerpool.bind(nMsgID, this, address, strdebug);
    }

    bool UnRgsMsgCall(const s32 nMsgID, memfun address) {
        return m_bundlerpool.unbind(nMsgID, this, address);
    }

    virtual void Disconnect(tcore::IKernel * pKernel) {
        m_bundlerpool.clear();
        OnConnectionBreak(pKernel);
    }

    virtual void Error(tcore::IKernel * pKernel, const s8 opt, void * pContext, const char * debug) {
        TASSERT(tcore::SO_CONNECT == opt, "wtf");
        if (tcore::SO_CONNECT == opt) {
            OnConnectFailed(pKernel);
        }
    }

    virtual void Connected(tcore::IKernel * pKernel) {
        OnConnected(pKernel);
    }

    virtual void OnConnected(tcore::IKernel * pKernel) = 0;
    virtual void OnConnectFailed(tcore::IKernel * pKernel) = 0;
    virtual void OnConnectionBreak(tcore::IKernel * pKernel) = 0;

private:
    tlib::bundler_pool<IConnectionSink, s32, tcore::IKernel *, const s32, const void *, const s32> m_bundlerpool;
};

#endif	/* __IConnectionSink_h_ */

