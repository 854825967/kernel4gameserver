#ifndef __OBJECTMGR_H__
#define __OBJECTMGR_H__

#include "IObjectMgr.h"
#include <map>
using namespace std;

class CObject;

class ObjectMgr : public IObjectMgr
{
public:
	virtual bool Initialize(IKernel * pKernel);
	virtual bool Launched(IKernel * pKernel);
	virtual bool Destroy(IKernel * pKernel);

	CObject* CreateObject(const string& strObjectType);

	virtual void TestModuleUnion() {
		ECHO("%ld", this);
	}

private:

	typedef CObject* (ObjectMgr::*CREATE_OBJ_FUN)(void);
	map<string, CREATE_OBJ_FUN> m_mapObjectCreator;

	CObject* CreateRole(void);
};



#endif