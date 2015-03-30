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
#include "ProtocolID.pb.h"
using namespace tcore;

class ClientConnectSink : public IConnectionSink {
public:
    virtual void OnConnected(tcore::IKernel * pKernel) {
        RGS_MSG_CALL(CLIENT_MSG_CHOOSE_SERVER_REQ, ClientConnectSink::OnClientChooseServerReq);
        RGS_MSG_CALL(CLIENT_MSG_LOGIN_REQ, ClientConnectSink::OnClientLoginReq);
    }

    virtual void OnConnectFailed(tcore::IKernel * pKernel) {

    }

    virtual void OnConnectionBreak(tcore::IKernel * pKernel) {

    }

    void OnClientChooseServerReq(tcore::IKernel *, const s32 nMsgID, const void * pContext, const s32 nSize) {
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
        SendMSG(SERVER_MSG_CHOOSE_SERVER_ASK, buff.c_str(), buff.size());
    }

    void OnClientLoginReq(tcore::IKernel *, const s32 nMsgID, const void * pContext, const s32 nSize) {

    }

    

private:

};

#endif	/* __ClientConnectSick_h_ */

