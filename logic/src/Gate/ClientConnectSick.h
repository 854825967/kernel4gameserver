/* 
 * File:   ClientConnectSick.h
 * Author: alax
 *
 * Created on March 3, 2015, 1:06 PM
 */

#ifndef __ClientConnectSick_h_
#define	__ClientConnectSick_h_

#include "IKernel.h"
#include "demo.pb.h"
using namespace tcore;

class SendConnectSick : public ITcpSocket {
public:
    virtual void Error(IKernel * pKernel, const s8 opt, void * pContext, const char * debug) {

    }

    virtual s32 Recv(IKernel * pKernel, const void * context, const s32 size) {
        return 0;
    }

    virtual void Disconnect(IKernel * pKernel) {

    }

    virtual void Connected(IKernel * pKernel) {
        Demo demo;
        demo.set_id(12580);
        s32 nMaxID = rand() % 1000;
        ECHO("send max count %d", nMaxID);
        for (s32 i=0; i<nMaxID; i++) {
            demo.add_context(tools::IntAsString(i) + "_test");
        }
        s32 size = sizeof(s32) + demo.ByteSize();

        Send(&size, sizeof(size));
        char * pBuff = NEW char[demo.ByteSize()];
        if (demo.SerializeToArray(pBuff, demo.ByteSize())) {
            Send(pBuff, demo.ByteSize());
        } else {
            TASSERT(false, "wtf");
        }
        delete[] pBuff;
    }
};

class ClientConnectSick : public ITcpSocket, public ITimer {
public:
    ClientConnectSick();
    ~ClientConnectSick();

    virtual void Error(IKernel * pKernel, const s8 opt, void * pContext, const char * debug);

    virtual s32 Recv(IKernel * pKernel, const void * context, const s32 size) {
        s32 nSize = *(s32 *)context;

        if (nSize >= size) {
            s64 lTick = tools::GetTimeMillisecond();
            const void * pBuff = (const char *)context + sizeof(s32);
            Demo demo;
            if (demo.ParseFromArray(pBuff, nSize - sizeof(s32))) {
                ECHO("demo proto id %d, max count %d", demo.id(), demo.context_size());
                for (s32 i=0; i<demo.context_size(); i++) {
                    demo.mutable_context(i);
                }
            } else {
                TASSERT(false, "wtf");
            }
            ECHO("use ltick %ld", tools::GetTimeMillisecond() - lTick);
        }

        return size;
    }

    virtual void Disconnect(IKernel * pKernel);
    virtual void Connected(IKernel * pKernel);

    virtual void OnStart(IKernel * pKernel, const s32 sTimerID, s64 lTimetick);
    
    //called before first OnTimer, do not affect OnTimer count
    virtual void OnTime(IKernel * pKernel, const s32 sTimerID, s64 lTimetick);

    //called after last OnTimer or remove timer, do not affect OnTimer count
    //nonviolent will be false if timer is removed by developer,  otherwise true 
    virtual void OnTerminate(IKernel * pKernel, const s32 sTimerID, bool nonviolent, s64 lTimetick);

private:
    s32 m_nRecvSize;
};

#endif	/* __ClientConnectSick_h_ */

