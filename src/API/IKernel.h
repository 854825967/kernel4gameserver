#ifndef __IKernel_h__
#define __IKernel_h__

#include "MultiSys.h"
#include "TStream.h"

class IModule;

namespace tcore {
    class IKernel;

    class IPipe {
    public:
        virtual ~IPipe(){}
        virtual void Send(const void * pContext, const s32 nSize) = 0;
        virtual void Close() = 0;
    };

    class ISocket {
    public:
        virtual ~ISocket(){}
        IPipe * m_pPipe;
    };

    class ITcpSession : public ISocket {
    public:
        virtual ~ITcpSession(){}

        virtual s32 OnRecv(IKernel * pKernel, const void * context, const s32 size) = 0; // return size that u use;

        virtual void OnDisconnect(IKernel * pKernel) = 0;
        virtual void OnConnected(IKernel * pKernel) = 0;
        virtual void OnConnectFailed(IKernel * pKernel) = 0;

        void Close() {
            m_pPipe->Close();
        }

        inline void Send(const void * context, const s32 size) {
            m_pPipe->Send(context, size);
        }

    };

    class ITcpServer : public ISocket {
    public:
        virtual ~ITcpServer(){}

        virtual ITcpSession * MallocConnection(IKernel * pKernel) = 0;
        virtual void Error(IKernel * pKernel, ITcpSession * pSession) = 0;

    };

    class ITimer {
    public:
        virtual ~ITimer() {}

        //will be called before first OnTimer, will not affect OnTimer count
        virtual void OnStart(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) = 0; 

        virtual void OnTime(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) = 0;

        //called after last OnTimer or remove timer, do not affect OnTimer count
        //nonviolent will be false if timer is removed by developer,  otherwise true 
        virtual void OnTerminate(IKernel * pKernel, const s32 sTimerID, bool nonviolent, s64 lTimetick) = 0;
    };

    class IKernel {
    public:
		//find logic module
        virtual IModule * FindModule(const char * name) = 0;

        // net interface
        virtual bool StartTcpServer(ITcpServer * server, const char * ip, const s32 port) = 0;
        virtual bool StartTcpClient(ITcpSession * client, const char * ip, const s32 port) = 0;

        // log interfac
        virtual void LogDebug(const char * debug) = 0;
        virtual void LogTrace(const char * debug) = 0;
        virtual void LogError(const char * debug) = 0;

        // tiemr interface 
        virtual bool StartTimer(s32 id, tcore::ITimer * timer, s64 interval, s64 delay = 0, s64 loop = -1) = 0; //-1 is forever
        virtual bool KillTimer(s32 id, ITimer * timer) = 0;
        virtual bool KillTimer(tcore::ITimer * timer) = 0;

        //shutdown kernel
        virtual void Shutdown() = 0;
    };
}

#endif //defined __IKernel_h__

