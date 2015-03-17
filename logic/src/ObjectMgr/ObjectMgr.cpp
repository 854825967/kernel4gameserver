#include "ObjectMgr.h"
#include "Object.h"
#include "AttributeMgr.h"
#include "Tools.h"

bool ObjectMgr::Initialize(IKernel * pKernel) {
	bool res = CAttributeMgr::Instance()->LoadAllConfig(string(tools::GetAppPath()) + "/logic_config/ObjectMgr/AttribConfig.xml");
	TASSERT(res, "Load logic_config/AttribConfig.xml error");
	return true;
}

bool ObjectMgr::Launched(IKernel * pKernel) {

	return true;
}

bool ObjectMgr::Destroy(IKernel * pKernel) {

	return true;
}

CObject* ObjectMgr::CreateObject(const string& strObjectType)
{
	CObject* pObj = new CObject;
	if (pObj && pObj->SetType(strObjectType.c_str()))
	{
		return pObj;
	}

	if (pObj)
		delete pObj;

	return NULL;
}
