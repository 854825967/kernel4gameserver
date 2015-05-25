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
#include "TPool.h"
#include "PublicHeader.h"
#include <list>
#include <map>
using namespace tcore;

class ClientConnectSink;
typedef std::list<ClientConnectSink *> SINK_LIST;
typedef std::map<s32, SINK_LIST> SINK_LIST_MAP;

class ClientConnectSink : public ITcpSession {
public:
    static ClientConnectSink * Create() {
        return s_oPoolClient.Create();
    }

    inline void Release() {
        s_oPoolClient.Recover(this);
    }

    ClientConnectSink() {
        m_bIsConnected = false;
        m_nGroupID = -1;
    }

    // return size that u use;
    virtual s32 OnRecv(IKernel * pKernel, const void * context, const s32 size) {
        s32 nLen = *(const s32 *)context;
        if (nLen > 4096) {
            Close();
            return 0;
        }

        if (nLen <= size && nLen >= sizeof(testmsgheader)) {
            testmsgheader * pHeader = (testmsgheader *)context;
            switch (pHeader->msgid) {
            case TEST_MSG_ID_SET_GROUP:
                {
                    TASSERT(m_nGroupID == -1 && m_bIsConnected, "wtf");
                    m_nGroupID = * (s32 *)((const char *)context + sizeof(testmsgheader));
                    SetGroupID(m_nGroupID);
                    break;
                }
            case TEST_MSG_ID_BROADCAST:
                {
                    Send(context, nLen);
                    //BroadCast(context, nLen);
                    break;
                }
            }

            return nLen;
        } else {
            return 0;
        }
    }

    virtual void OnDisconnect(IKernel * pKernel) {
        s_nLinkCount--;
        ECHO("s_nLinkCount %d", s_nLinkCount);
        TASSERT(s_nLinkCount >= 0 && m_bIsConnected == true, "wtf");
        m_bIsConnected = false;
    }

    virtual void OnConnected(IKernel * pKernel) {
        s_nLinkCount++;
        ECHO("s_nLinkCount %d", s_nLinkCount);
        TASSERT(s_nLinkCount > 0 && m_bIsConnected == false, "wtf");
        m_bIsConnected = true;
    }

    virtual void OnConnectFailed(IKernel * pKernel) {

    }

private:
    void SetGroupID(s32 groupid) {
        m_nGroupID = groupid;
        SINK_LIST_MAP::iterator mapItor = s_oGroupMap.find(groupid);
        if (mapItor == s_oGroupMap.end()) {
            s_oGroupMap.insert(make_pair(groupid, SINK_LIST()));
            mapItor = s_oGroupMap.find(groupid);
        }

        mapItor->second.push_back(this);
    }

    void BroadCast(const void * context, const s32 len) {
        SINK_LIST_MAP::iterator mapItor = s_oGroupMap.begin();
        SINK_LIST_MAP::iterator mapIEnd = s_oGroupMap.end();

        while (mapItor != mapIEnd) {
            SINK_LIST::iterator listItor = mapItor->second.begin();
            SINK_LIST::iterator listIEnd = mapItor->second.end();

            while (listItor != listIEnd) {
                if ((*listItor)->m_bIsConnected) {
                    if (*listItor != this) {
                        (*listItor)->Send(context, len);
                    }
                    listItor++;
                } else {
                    (*listItor)->Release();
                    listItor = mapItor->second.erase(listItor);
                }
            }

            mapItor++;
        }
    }

private:
    bool m_bIsConnected;
    s32 m_nGroupID;

    static tlib::TPool<ClientConnectSink> s_oPoolClient;
    static s32 s_nLinkCount;
    static SINK_LIST_MAP s_oGroupMap;
};

#endif	/* __ClientConnectSick_h__ */

