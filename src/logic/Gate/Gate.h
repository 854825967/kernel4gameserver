/* 
 * File:   gate.h
 * Author: alax
 *
 * Created on March 3, 2015, 10:33 AM
 */

#ifndef __Gate_h__
#define	__Gate_h__

#include "IGate.h"
#include "IKernel.h"
#include "TBundler.h"
#include "CData.h"
#include "ClientConnectSink.h"

class ShutDownServer : public ITcpServer{
public:
    virtual ITcpSession * MallocConnection(IKernel * pKernel) {
        pKernel->Shutdown();
        return NEW ClientConnectSink;
    }

    virtual void Error(IKernel * pKernel, ITcpSession * pSession) {

    }

};

class Gate : public IGate, ITcpServer {
public:
    virtual bool Initialize(IKernel * pKernel);
    virtual bool Launched(IKernel * pKernel);
    virtual bool Destroy(IKernel * pKernel);        
    
    virtual ITcpSession * MallocConnection(IKernel * pKernel);
    virtual void Error(IKernel * pKernel, ITcpSession * pSession);
};

#endif	/* GATE_H */

