/* 
 * File:   ITimermgr.h
 * Author: alax
 *
 * Created on March 5, 2015, 11:04 AM
 */

#ifndef __ITimermgr_h__
#define	__ITimermgr_h__

#include "ICore.h"
#include "IKernel.h"

class ITimermgr : public ICore {
public:
    // tiemr interface 
    virtual bool StartTimer(s32 id, tcore::ITimer * timer, s64 interval, s64 delay, s64 loop) = 0;
    virtual bool KillTimer(s32 id, tcore::ITimer * timer) = 0;
    virtual bool KillTimer(tcore::ITimer * timer) = 0;
    
    virtual s64 Dotimer() = 0;
};

#endif	/* __ITimermgr_h__ */

