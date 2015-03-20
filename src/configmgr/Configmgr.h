/* 
 * File:   Configmgr.h
 * Author: alax
 *
 * Created on March 2, 2015, 2:16 PM
 */

#ifndef __Configmgr_h__
#define	__Configmgr_h__

#include "IConfigmgr.h"

class Configmgr : public IConfigmgr {
public:
    static IConfigmgr * getInstance();

    virtual bool Redry();
    virtual bool Initialize();
    virtual bool Destory();

    virtual inline const sCoreConfig * GetCoreConfig();
    virtual inline const sModuleConfig * GetModuleConfig();
private:
    bool LoadCoreConfig();
    bool LoadModuleConfig();

private:
    Configmgr();
    virtual ~Configmgr();

private:
    sCoreConfig m_oCoreConfig;
    sModuleConfig m_oModuleConfig;
};

#endif	/* __Configmgr_h__ */

