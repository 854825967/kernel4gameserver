#include "epoll_worker.h"
#include "IKernel.h"
using namespace tcore;

void epoll_worker::Run() {
    while (true) {
        epoller_data * p;
        if (m_queue.Read(p)) {
            switch (p->opt) {
                case SO_TCPRECV:
                {
                    ITcpSocket * ts = (ITcpSocket *)p->user_ptr;
                    ts->DoIO(p->flags, m_pEpoller);
                    break;
                }
//                case SO_UDPIO:
//                {
//                    break;
//                }
                default:
                    TASSERT(false, "bad opt");
                    break;
            }
        } else {
            CSLEEP(1);
        }
    }
}
