#ifndef __IKernel_h__
#define __IKernel_h__

#include "MultiSys.h"
#include "TStream.h"

class IModule;

#ifdef _WIN32
struct iocp_event;
#endif //_WIN32

namespace tcore {

#define BUFF_SIZE (16 * 1024)
    class IKernel;

    enum eSocketOpt {
        SO_ACCEPT,
        SO_TCPRECV,
        SO_TCPSEND,
        SO_CONNECT,

        //FOR UDP
        //SO_UDPIO,
    };

    enum eSocketStatus {
        SS_UNINITIALIZE = 0,
        SS_ESTABLISHED,
        SS_WAITCLOSE,
    };

    class ISocket {
    public:

        ISocket() {
            Clear();
        }

        void Clear() {
            socket_handler = -1;
            m_nStatus = SS_UNINITIALIZE;
            m_bRecvPending = false;
            m_bSendPending = false;
        }

        virtual void Error(IKernel * pKernel, const s8 opt, void * pContext, const char * debug) = 0; //code:0=success

        inline void InitAddr(const char * _ip, const s32 _port) {
            memset(ip, 0, sizeof (ip));
            SafeSprintf(ip, sizeof (ip), _ip);
            port = _port;
        }

        inline void Close() {
            if (m_nStatus == SS_ESTABLISHED) {
                m_nStatus = SS_WAITCLOSE;
            }
        }

        inline s8 GetStatus() {
            return m_nStatus;
        }

    public:
        struct sockaddr_in m_addr;
        s64 socket_handler;
        char ip[32];
        s32 port;
        s8 m_nStatus;
        tlib::CLockUnit m_lockAccept;

#ifdef _WIN32
        bool m_bRecvPending;
        bool m_bSendPending;
#endif //_WIN32
    };

    class ITcpSocket : public ISocket {
    public:
        virtual s32 Recv(IKernel * pKernel, const void * context, const s32 size) = 0; // return size that u use;

        virtual void Disconnect(IKernel * pKernel) = 0;
        virtual void Connected(IKernel * pKernel) = 0;

        inline void Send(const void * context, const s32 size) {
            if (m_nStatus == SS_ESTABLISHED) {
                m_sendStream.in(context, size);
            }
        }

        void DoConnect(s32 flags, void * pContext);

#ifdef linux
        void DoIO(s32 flags, void * pContext);
#endif //linux

        inline void Clear() {
            m_recvStream.clear();
            m_sendStream.clear();
        }

#ifdef WIN32
        bool DoRecv(iocp_event * pEvent, HANDLE hCompletionPort);
        bool DoSend(iocp_event * pEvent, HANDLE hCompletionPort);
#endif //WIN32

    public:
        tlib::TStream<BUFF_SIZE, true> m_recvStream;
        tlib::TStream<BUFF_SIZE, true> m_sendStream;
    };

    class ITcpServer : public ISocket {
    public:
        virtual ITcpSocket * MallocConnection(IKernel * pKernel) = 0;

        void DoAccept(s32 flags, void * pContext);

    };

    class ITimer {
    public:
        virtual void OnStart(IKernel * pKernel, const s32 sTimerID, s64 lTimetick) = 0; //called before first OnTimer, do not affect OnTimer count
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
        virtual bool StartTcpServer(ITcpServer * sever) = 0;
        virtual bool StartTcpClient(ITcpSocket * client) = 0;

        // tiemr interface 
        virtual bool StartTimer(s32 id, tcore::ITimer * timer, s64 interval, s64 delay = 0, s64 loop = -1) = 0; //-1 is forever
        virtual bool KillTimer(s32 id, ITimer * timer) = 0;
        virtual bool KillTimer(tcore::ITimer * timer) = 0;

        //shutdown kernel
        virtual void Shutdown() = 0;
    };
}

#endif //defined __IKernel_h__

