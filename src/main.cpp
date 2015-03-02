#include "MultiSys.h"
#include "TStream.h"
#include "epoller.h"
#include "IKernel.h"
#include "TQueue.h"
#include <fcntl.h>
#include <netinet/in.h>

using namespace tlib;

class Client1 : public tcore::ITcpSocket {
public:

    void Error(const s8 opt, const s32 code) {
        
    }

    void Disconnect() {
        ECHO("disconnect");
        delete this;
    }

    void Connected() {
    }

#define TEST_SEND_LEN (1024 * 1024 * 2)

    s32 Recv(void * context, const s32 size) {
        ECHO("%s, %d", (const char *) context, size);
        char pbuff[TEST_SEND_LEN] = {0};
        memset(pbuff, 'h', TEST_SEND_LEN);
        Send(pbuff, TEST_SEND_LEN);
        Close();
        return 0;
    }
};

static s32 connect_count = 0;

class Client : public tcore::ITcpSocket {
public:

    void Error(const s8 opt, const s32 code) {
        
    }

    void Disconnect() {
        connect_count--;
        delete this;
    }

    void Connected() {
        connect_count++;
        Send("hello server", strlen("hello server") + 1);
    }

    s32 Recv(void * context, const s32 size) {
        ECHO("recv data len %d", size);
        //CSLEEP(5000);
        return 0;
    }
};

class TSocket : public tcore::ITcpServer {
public:

    void Error(const s8 opt, const s32 code) {
        
    }

    tcore::ITcpSocket * MallocConnection() {
        return NEW Client1;
    }
};

class ITelnet : public tcore::ITcpSocket {

    void Error(const s8 opt, const s32 code) {
        
    }

    void Disconnect() {
        delete this;
    }

    void Connected() {
        
    }

    s32 Recv(void * context, const s32 size) {
        char buff[128] = {0};
        SafeSprintf(buff, sizeof(buff), "connection count %d", connect_count);
        
        Send(buff, strlen(buff) + 1);
        
        return 0;
    }
};

class IConsole : public tcore::ITcpServer {

    void Error(const s8 opt, const s32 code) {
        
    }

    tcore::ITcpSocket * MallocConnection() {
        return NEW ITelnet;
    }
};

class Test : public tlib::CThread {
public:

    void Run() {

        while (true) {
            Client * pClient = NEW Client;
            pClient->Initialize("127.0.0.1", 12899);
            p->AddClient(pClient);
            CSLEEP(10000);
        }

    }

    epoller * p;
};

int main() {
    TQueue<int, false, 512> queue;

    epoller ep;
    ep.Initialize();

    TSocket * pS = NEW TSocket;
    pS->Initialize("0.0.0.0", 12899);
    ep.AddServer(pS);

    IConsole * pConsole = NEW IConsole;
    pConsole->Initialize("0.0.0.0", 12580);
    ep.AddServer(pConsole);
    
    Test clients1;
    clients1.p = &ep;
    clients1.Start(2);
    
    while (true) {
        ep.DonetIO(100);
    }

    return 0;
}

