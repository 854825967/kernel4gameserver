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
#include "CRand.h"
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

class Timer : public ITimer {
public:
    Timer(s32 nInterval, s32 nDelay) {
        m_nInterval = nInterval;
        m_lNextTick = tools::GetTimeMillisecond() + nDelay;
    }

    virtual void OnStart(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {
        m_nHitCount = 0;
        m_nPrintCount = 10;
    } 

    virtual void OnTime(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {
        s64 lTick = tools::GetTimeMillisecond() - m_lNextTick;
        if (lTick > 50 || lTick < -50) {
            ECHO("%ld", lTick);
        }
        m_lNextTick += m_nInterval;
        m_nHitCount++;
//         if (m_nHitCount % m_nPrintCount == 0) {
//             ECHO("hit count %d", m_nHitCount);
//             m_nPrintCount *= 10;
//         }
    }

    virtual void OnTerminate(IKernel * pKernel, const s32 sTimerID, bool nonviolent, s64 lTimetick) {

    }

private:
    s32 m_nInterval;
    s64 m_lNextTick;
    s32 m_nHitCount;
    s32 m_nPrintCount;
};

class Gate : public IGate, ITcpServer, ITimer, CRand{
public:
    virtual bool Initialize(IKernel * pKernel);
    virtual bool Launched(IKernel * pKernel) {
        pKernel->StartTcpServer(this, "0.0.0.0", 12588);

        pKernel->StartTimer(1, this, 100, 0, 100);
        return true;
    }

    virtual bool Destroy(IKernel * pKernel);        
    
    virtual ITcpSession * MallocConnection(IKernel * pKernel);
    virtual void Error(IKernel * pKernel, ITcpSession * pSession);

    virtual void OnStart(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {
        
    } 

    virtual void OnTime(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {
        static s32 nTimerCount = 0;
        //ECHO("%ld", tools::GetTimeMillisecond() - lTimetick);
        SetSeed(tools::GetTimeMillisecond());
        for (s32 i=0; i<10; i++) {
            s32 nInterval = 200;
            s32 nDelay = Rand() % 100;
            Timer * p = NEW Timer(nInterval, nDelay);
            pKernel->StartTimer(0, p, nInterval, nDelay);

            nInterval = 400;
            nDelay = Rand() % 100;
            p = NEW Timer(nInterval, nDelay);
            pKernel->StartTimer(0, p, nInterval, nDelay);

            nInterval = 600;
            nDelay = Rand() % 100;
            p = NEW Timer(nInterval, nDelay);
            pKernel->StartTimer(0, p, nInterval, nDelay);

            nInterval = 800;
            nDelay = Rand() % 100;
            p = NEW Timer(nInterval, nDelay);
            pKernel->StartTimer(0, p, nInterval, nDelay);

            nInterval = 1000;
            nDelay = Rand() % 100;
            p = NEW Timer(nInterval, nDelay);
            pKernel->StartTimer(0, p, nInterval, nDelay);

            nInterval = 1200;
            nDelay = Rand() % 100;
            p = NEW Timer(nInterval, nDelay);
            pKernel->StartTimer(0, p, nInterval, nDelay);

            nInterval = 1500;
            nDelay = Rand() % 100;
            p = NEW Timer(nInterval, nDelay);
            pKernel->StartTimer(0, p, nInterval, nDelay);

            nInterval = 2000;
            nDelay = Rand() % 100;
            p = NEW Timer(nInterval, nDelay);
            pKernel->StartTimer(0, p, nInterval, nDelay);

            nInterval = 5000;
            nDelay = Rand() % 100;
            p = NEW Timer(nInterval, nDelay);
            pKernel->StartTimer(0, p, nInterval, nDelay);

            nInterval = 20000;
            nDelay = Rand() % 100;
            p = NEW Timer(nInterval, nDelay);
            pKernel->StartTimer(0, p, nInterval, nDelay);

            nInterval = 3600000;
            nDelay = Rand() % 100;
            p = NEW Timer(nInterval, nDelay);
            pKernel->StartTimer(0, p, nInterval, nDelay);
        }

        nTimerCount += 100;
        if (nTimerCount % 500 == 0) {
            ECHO("TIMER COUNT %d", nTimerCount);
        }
    }

    virtual void OnTerminate(IKernel * pKernel, const s32 sTimerID, bool nonviolent, s64 lTimetick) {

    }
};

#endif	/* GATE_H */

