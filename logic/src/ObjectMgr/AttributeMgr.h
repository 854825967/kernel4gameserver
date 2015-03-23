#ifndef _ATTRIBUTEMGR_H__
#define _ATTRIBUTEMGR_H__

#include "MultiSys.h"
#include <string>
#include <vector>
#include <map>
#include "CData.h"

using namespace std;

#define MAX_ATTR_STRING_LEN		2048

struct attr_info 
{
	attr_info()
		: type (DATA_TYPE_INVALID)
		, seripos (0)
		, length (0)
	{}
	s8	type;
	size_t seripos;
	size_t length;
};


typedef map<string, attr_info> MAP_ATTR_INFO;
struct ATTR_CONFIG {
	ATTR_CONFIG()
		: nBuffLen(0)
	{}
	MAP_ATTR_INFO mapAttrInfo;
	string strBaseType;
	size_t nBuffLen;
};
typedef map<string, ATTR_CONFIG> MAP_ATTR_CONFIG;
typedef map<string, s8> MAP_ATTR_TYPE;
typedef map<string, s16> MAP_OBJ_TYPE;

class CAttributeMgr
{
public:
	static CAttributeMgr* Instance(void)
	{
		static CAttributeMgr* p = NULL;
		if (p == NULL)
		{
			p = NEW CAttributeMgr;
		}
		return p;
	}
	
	bool LoadAllConfig(const string& configfile);

	attr_info GetAttrInfo(const string& objName, const string& attrName);

	ATTR_CONFIG* GetAttrConfig(const string& objName);

	s16 GetObjType(const string& strObjType);

private:
	CAttributeMgr(void);
	bool LoadConfig(const string& objType);

	s8 GetType(const string& strType);

	MAP_ATTR_CONFIG m_mapAttrConfig;
	MAP_ATTR_TYPE m_mapAttrType;
	MAP_OBJ_TYPE m_mapObjType;
};

#endif