#ifndef __INetengine_h__
#define __INetengine_h__

#include "IKernel.h"
#include "ICore.h"

class INetengine : public ICore {
public:
    virtual bool AddServer(tcore::ITcpServer * server, const char * ip, const s32 port) = 0;
    virtual bool AddClient(tcore::ITcpSession * client, const char * ip, const s32 port) = 0;
    virtual s64 Processing(s64 overtime) = 0;
};

#endif  //__INetengine_h__
