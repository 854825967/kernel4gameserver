/* 
 * File:   ClientConnectSick.h
 * Author: alax
 *
 * Created on March 3, 2015, 1:06 PM
 */

#ifndef __ClientConnectSick_h_
#define	__ClientConnectSick_h_

#include "IConnectionSink.h"
#include "demo.pb.h"
using namespace tcore;

class ClientConnectSink : public IConnectionSink {
public:
    virtual void OnConnected(tcore::IKernel * pKernel) {
        RGS_MSG_CALL(1, ClientConnectSink::OnClientTest);
    }

    virtual void OnConnectFailed(tcore::IKernel * pKernel) {

    }

    virtual void OnConnectionBreak(tcore::IKernel * pKernel) {

    }

    virtual void OnClientTest(tcore::IKernel *, const s32 nMsgID, const void * pContext, const s32 nSize) {
        DemoClientReq req;
        if ( !req.ParseFromString(string((const char *)pContext)) ) {
            ECHO_ERROR("error msg format, msg id : %d", nMsgID);
            return;
        }

        ECHO("client msg:%s", req.text().c_str());
        DemoServerAsk ask;
        ask.set_text("come on baby");
        string buff;
        ask.SerializeToString(&buff);
        SendMSG(1, buff.c_str(), buff.size());
    }

private:

};

#endif	/* __ClientConnectSick_h_ */

