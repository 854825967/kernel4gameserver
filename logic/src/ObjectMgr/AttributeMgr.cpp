#include "AttributeMgr.h"
#include "tinyxml/tinyxml.h"
#include "Tools.h"
#include <vector>
#include <iostream>

using namespace std;

CAttributeMgr::CAttributeMgr(void)
{}

void CAttributeMgr::Init(void)
{
	m_mapType["s8"] = ATTR_TYPE_S8;
	m_mapType["s16"] = ATTR_TYPE_S16;
	m_mapType["s32"] = ATTR_TYPE_S32;
	m_mapType["s64"] = ATTR_TYPE_S64;
	m_mapType["string"] = ATTR_TYPE_STRING;
	m_mapType["blob"] = ATTR_TYPE_BLOB;
}

bool CAttributeMgr::LoadAllConfig(const string& configfile)
{
	Init();

	MAP_CONFIG mapConfig = m_mapConfig;
	m_mapConfig.clear();

	vector<string> vObjTypes;
	
	TiXmlDocument configdoc(configfile.c_str());
	if (!configdoc.LoadFile()) {
		ECHO_ERROR("Load xml file error %s", configfile.c_str());
		return false;
	}

	TiXmlNode* ndRoot = configdoc.RootElement();
	if (!ndRoot) {
		ECHO_ERROR("Load xml root error %s", configfile.c_str());
		return false;
	}

	for (TiXmlNode* ndConfigFileName = ndRoot->FirstChild("config"); ndConfigFileName; ndConfigFileName = ndConfigFileName->NextSibling("config"))
	{
		TiXmlElement* eleConfigFileName = ndConfigFileName->ToElement();
		string strObjType;
		if (eleConfigFileName->Attribute("typename"))
		{
			strObjType = eleConfigFileName->Attribute("typename");
		}
		if (strObjType.empty()) {
			return false;
		}
		vObjTypes.push_back(strObjType);
	}

	for (vector<string>::iterator itr = vObjTypes.begin(); itr != vObjTypes.end(); ++itr) {
		if (!LoadConfig(*itr)) {
			m_mapConfig = mapConfig;
			cout<<"load object config failed ... object type : "<<itr->c_str()<<endl;
			return false;
		}
	}

	return true;
}

bool CAttributeMgr::LoadConfig(const string& objType)
{
	string objconfigfile = string(tools::GetAppPath()) + "/logic_config/ObjectMgr/" + objType + ".xml";

	TiXmlDocument doc(objconfigfile.c_str());
	if (!doc.LoadFile())
	{
		return false;
	}

	TiXmlNode* ndRoot = doc.RootElement();
	if (!ndRoot)
	{
		return false;
	}

	s32 nStartPos = 0;

	for (TiXmlNode* ndAttr = ndRoot->FirstChild("attr"); ndAttr; ndAttr = ndAttr->NextSibling("attr"))
	{
		TiXmlElement* eleAttr = ndAttr->ToElement();
		string strAttrName;
		attr_info info;
		if (eleAttr->Attribute("name"))
			strAttrName = eleAttr->Attribute("name");
		if (eleAttr->Attribute("type"))
			info.type = GetType(eleAttr->Attribute("type"));

		switch (info.type)
		{
		case ATTR_TYPE_S8:
			info.length = sizeof(s8);
			break;
		case ATTR_TYPE_S16:
			info.length = sizeof(s16);
			break;
		case ATTR_TYPE_S32:
			info.length = sizeof(s32);
			break;
		case ATTR_TYPE_S64:
			info.length = sizeof(s64);
			break;
		case ATTR_TYPE_STRING:
		case ATTR_TYPE_BLOB:
			{
				if (eleAttr->Attribute("length"))
					info.length = atoi(eleAttr->Attribute("length"));
				if (info.length == 0)
					return false;
			}
			break;
		default:
			{
				// 类型不可识别
				TASSERT(false, "format error");
				return false;
			}
			break;
		}

		info.seripos = nStartPos;
		nStartPos += info.length;

		if (nStartPos > MAX_ATTR_BUF_LEN)
		{
			return false;
		}

		m_mapConfig[objType][strAttrName] = info;
	}

	return true;
}

s8 CAttributeMgr::GetType(const string& strType)
{
	MAP_TYPE::iterator itr = m_mapType.find(strType.c_str());
	if (itr != m_mapType.end())
	{
		return itr->second;
	}
	return ATTR_TYPE_INVALID;
}

attr_info CAttributeMgr::GetAttrInfo(const string& objName, const string& attrName)
{
	return m_mapConfig[objName][attrName];
}

MAP_ATTR* CAttributeMgr::GetAttrConfig(const string& objtype)
{
	MAP_CONFIG::iterator itr = m_mapConfig.find(objtype.c_str());
	if (itr != m_mapConfig.end())
	{
		return &(itr->second);
	}
	return NULL;
}