#ifndef __INetengine_h__
#define __INetengine_h__

#include "ICore.h"
#include "IKernel.h"

class INetengine : public ICore {
public:
    virtual bool AddServer(tcore::ITcpServer * server) = 0;
    virtual bool AddClient(tcore::ITcpSocket * client) = 0;
    virtual s64 DonetIO(s64 overtime) = 0;
    virtual void Run() = 0;
};

#endif  //__INetengine_h__
