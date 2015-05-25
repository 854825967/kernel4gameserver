#ifndef TCALLBACK_H
#define TCALLBACK_H
#include "MultiSys.h"
#include "CHashMap.h"
#include "Tools.h"
#include <list>
using namespace std;
namespace tlib {

    template<typename type, const s32 info_size = 256>
    struct CallInfo{
        type value;
        char info[info_size];
        CallInfo() {
            memset(this, 0, sizeof(*this));
        }

        CallInfo(type val, const char * pInfo) {
            value = val;
            SafeSprintf(info, sizeof(info), "%s", pInfo);
        }

        CallInfo<type, info_size> & operator = (const CallInfo<type, info_size> & target) {
            memcpy(this, &target, sizeof(target));
            return *this;
        }

        bool operator == (const CallInfo<type, info_size> & target) {
            return (this->value == target.value);
        }

        bool operator < (const CallInfo<type, info_size> & target) {
            return (this->value < target.value);
        }
    };

    template<typename ID_TYPE, typename TYPE_CALL, typename... Args>
    class TCallBack {
        typedef CallInfo<TYPE_CALL, 256> CALLINFO;
        typedef list<CALLINFO> CALLPOOL;
        typedef CHashMap<ID_TYPE, CALLPOOL> MAP_CALLPOOL;

    public:
        TCallBack() {
            m_mapContainer.clear();
        }

        ~TCallBack() {
            m_mapContainer.clear();
        }

        bool RegisterCall(const ID_TYPE id, const TYPE_CALL pFun, const char * pInfo = "no info") {
            typename MAP_CALLPOOL::iterator ifind = m_mapContainer.find(id);
            if (ifind == m_mapContainer.end()) {
                m_mapContainer.insert(make_pair(id, CALLPOOL()));
                ifind = m_mapContainer.find(id);
            }

            typename CALLPOOL::iterator itor = std::find(ifind->second.begin(), ifind->second.end(), CALLINFO(pFun, NULL));
            if (itor != ifind->second.end()) {
                ASSERT(false);
                return false;
            }

            ifind->second.push_back(CALLINFO(pFun, pInfo));
            ECHO("Register Call %s", pInfo);

            return true;
        }

        bool UnRgsCall(const ID_TYPE id, const TYPE_CALL pFun) {
            typename MAP_CALLPOOL::iterator ifind = m_mapContainer.find(id);
            if (ifind == m_mapContainer.end()) {
                m_mapContainer.insert(make_pair(id, CALLPOOL()));
                ifind = m_mapContainer.find(id);
            }

            typename CALLPOOL::iterator itor = std::find(ifind->second.begin(), ifind->second.end(), CALLINFO(pFun, NULL));
            if (itor == ifind->second.end()) {
                return false;
            }
            ECHO("Unregister Call %s", itor->info);
            ifind->second.erase(itor);
            return true;
        }

        void Call(const ID_TYPE id, Args... a) {
            typename MAP_CALLPOOL::iterator ifind = m_mapContainer.find(id);

            if (ifind == m_mapContainer.end()) {
                return;
            }

            typename CALLPOOL::iterator itor = ifind->second.begin();
            typename CALLPOOL::iterator iend = ifind->second.end();

            while (itor != iend) {
#ifdef _DEBUG
                s64 lTick = ::GetCurrentTimeTick();
                char szInfo[256] = {0};
                SafeSprintf(szInfo, sizeof(szInfo), "%s", itor->info);
#endif //_DEBUG
                typename CALLPOOL::iterator icurrent = itor++;
                (icurrent->value)(a...);
#ifdef _DEBUG
                lTick = ::GetCurrentTimeTick() - lTick;
                if (lTick >= 10) {
                    ECHO_WARN("%s use tick %lld", szInfo, lTick);
                }
#endif //_DEBUG
            }
        }

    private:
        MAP_CALLPOOL m_mapContainer;
    };
}
#endif //TCALLBACK_H
