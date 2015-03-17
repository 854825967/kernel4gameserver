/* 
 * File:   ClientConnectSick.h
 * Author: alax
 *
 * Created on March 3, 2015, 1:06 PM
 */

#ifndef __ClientConnectSick_h_
#define	__ClientConnectSick_h_

#include "IKernel.h"
using namespace tcore;


class ClientConnectSick : public ITcpSocket {
public:
    ClientConnectSick();

    virtual void Error(IKernel * pKernel, const s8 opt, const s32 code);
    virtual s32 Recv(IKernel * pKernel, void * context, const s32 size);

    virtual void Disconnect(IKernel * pKernel);
    virtual void Connected(IKernel * pKernel);


private:

};

#endif	/* __ClientConnectSick_h_ */

