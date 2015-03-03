/* 
 * File:   gate.h
 * Author: alax
 *
 * Created on March 3, 2015, 10:33 AM
 */

#ifndef __Gate_h__
#define	__Gate_h__

#include "IGate.h"
#include "IKernel.h"

class Gate : public IGate, ITcpServer {
public:
    virtual bool Initialize(IKernel * pKernel);
    virtual bool Launched(IKernel * pKernel);
    virtual bool Destroy(IKernel * pKernel);
    virtual void Error(const s8 opt, const s32 code);
    virtual ITcpSocket * MallocConnection();
};

#endif	/* GATE_H */

