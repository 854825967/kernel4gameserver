/* 
 * File:   IObjectMgr.h
 * Author: alax
 *
 * Created on March 3, 2015, 10:46 AM
 */

#ifndef __IObjectMgr_h__
#define	__IObjectMgr_h__

#include "IModule.h"

class IObject {
public:

};

class IObjectMgr : public IModule {
public:
	virtual void TestModuleUnion() = 0;
// 	virtual IObject * CreateObject() = 0;
// 	virtual IObject * FindObjcet() = 0;
};

#endif	/* __IObjectMgr_h__ */

