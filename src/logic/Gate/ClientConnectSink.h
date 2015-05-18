/* 
 * File:   ClientConnectSick.h
 * Author: alax
 *
 * Created on March 3, 2015, 1:06 PM
 */

#ifndef __ClientConnectSick_h__
#define	__ClientConnectSick_h__
#include "IKernel.h"
#include "CHashMap.h"
using namespace tcore;
class ClientConnectSink;
typedef CHashMap<ClientConnectSink *, ClientConnectSink *> MAP_CONNECTSINK;

static MAP_CONNECTSINK s_map;

static s32 s_count = 0;
static ThreadID s_id = -1;

class ClientConnectSink : public ITcpSession {
public:
    ClientConnectSink() {
        m_bConnected = false;
    }

    // return size that u use;
    virtual s32 OnRecv(IKernel * pKernel, const void * context, const s32 size) {
        return size;
    }

    virtual void OnDisconnect(IKernel * pKernel) {
        pKernel->LogDebug("OnDisconnect");
        TASSERT(tools::GetCurrentThreadID() == s_id, "wtf");

        MAP_CONNECTSINK::iterator itor = s_map.find(this);
        TASSERT(itor != s_map.end(), "wtf");
        s_map.erase(itor);
        delete this;
    }

    virtual void OnConnected(IKernel * pKernel) {
        if (-1 == s_id) {
            s_id = tools::GetCurrentThreadID();
        }

        TASSERT(tools::GetCurrentThreadID() == s_id, "wtf");
        TASSERT(m_bConnected == false, "wtf");

        m_bConnected = true;

        pKernel->LogDebug("OnConnected");
        char buff[1024 * 100] = {0};
        memset(buff, 'h', sizeof(buff));
        Send(buff, sizeof(buff));
        Close();

        s_map.insert(make_pair(this, this));
    }

    virtual void OnConnectFailed(IKernel * pKernel) {

    }

private:
    s64 m_lRecvSize;
    s64 m_lTick;
    s64 m_bConnected;
};

#endif	/* __ClientConnectSick_h__ */

