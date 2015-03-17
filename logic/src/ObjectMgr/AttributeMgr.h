#ifndef _ATTRIBUTEMGR_H__
#define _ATTRIBUTEMGR_H__

#include "MultiSys.h"
#include <string>
#include <map>

using namespace std;

#define MAX_ATTR_BUF_LEN	32767

enum E_AttrType
{
	ATTR_TYPE_INVALID,
	ATTR_TYPE_S8,
	ATTR_TYPE_S16,
	ATTR_TYPE_S32,
	ATTR_TYPE_S64,
	ATTR_TYPE_STRING,
	ATTR_TYPE_BLOB,
};

struct attr_info 
{
	attr_info()
		: type (ATTR_TYPE_INVALID)
		, seripos (0)
		, length (0)
	{}
	s8	type;
	s32 seripos;
	s32 length;
};


typedef map<string, attr_info> MAP_ATTR;
typedef map<string, MAP_ATTR> MAP_CONFIG;

typedef map<string, s8> MAP_TYPE;

class CAttributeMgr
{
public:
	static CAttributeMgr* Instance(void)
	{
		static CAttributeMgr* p = NULL;
		if (p == NULL)
		{
			p = new CAttributeMgr;
		}
		return p;
	}

	void Init(void);

	bool LoadAllConfig(const string& configfile);


	attr_info GetAttrInfo(const string& objName, const string& attrName);

	MAP_ATTR* GetAttrConfig(const string& objName);


private:
	CAttributeMgr(void);
	bool LoadConfig(const string& objType);

	s8 GetType(const string& strType);

	MAP_CONFIG m_mapConfig;
	MAP_TYPE m_mapType;
};

#endif