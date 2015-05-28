#ifndef __Client_h__
#define __Client_h__
#include "IKernel.h"
#include "TPool.h"
#include "PublicHeader.h"
#include "CRand.h"
using namespace tcore;

class Client : public ITcpSession, ITimer, CRand {
public:
    Client() {
        m_bIsConnected = false;
        m_nGroupID = -1;
    }

    static Client * Create() {
        return s_oClientPool.Create();
    }

    void Release() {
        TASSERT(m_bIsConnected == false, "wtf");
        s_oClientPool.Recover(this);
    }

    //will be called before first OnTimer, will not affect OnTimer count
    virtual void OnStart(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {

    }

    virtual void OnTime(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) {

    }

    //called after last OnTimer or remove timer, do not affect OnTimer count
    //nonviolent will be false if timer is removed by developer,  otherwise true 
    virtual void OnTerminate(IKernel * pKernel, const s32 sTimerID, bool nonviolent, s64 lTimetick) {

    }

    virtual s32 OnRecv(IKernel * pKernel, const void * context, const s32 size) {
        return size;
    }

    virtual void OnDisconnect(IKernel * pKernel) {
        s_nLinkCount--;
        TASSERT(s_nLinkCount >= 0 && m_bIsConnected == true, "wtf");
        m_bIsConnected = false;
    }

    virtual void OnConnected(IKernel * pKernel) {
        SetSeed(tools::GetTimeMillisecond());
        m_nGroupID = s_nLinkCount % 40;
        s_nLinkCount++;
        TASSERT(s_nLinkCount > 0 && m_bIsConnected == false, "wtf");
        m_bIsConnected = true;
    }

    virtual void OnConnectFailed(IKernel * pKernel) {
        TASSERT(m_bIsConnected == false, "wtf");
    }
private:
    bool m_bIsConnected;
    s32 m_nGroupID;

    static s32 s_nLinkCount;
    static tlib::TPool<Client> s_oClientPool;
};

#endif //__Client_h__
