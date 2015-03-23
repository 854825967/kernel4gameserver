#include "AttributeMgr.h"
#include "tinyxml/tinyxml.h"
#include "Tools.h"
#include <vector>
#include <iostream>

using namespace std;

CAttributeMgr::CAttributeMgr(void)
{}

bool CAttributeMgr::LoadAllConfig(const string& configfile)
{
	MAP_ATTR_CONFIG mapAttrConfig = m_mapAttrConfig;
	m_mapAttrConfig.clear();

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

	s16 nObjTypeIndex = 0;

	for (TiXmlNode* ndConfigFileName = ndRoot->FirstChild("config"); ndConfigFileName; ndConfigFileName = ndConfigFileName->NextSibling("config")) {
		TiXmlElement* eleConfigFileName = ndConfigFileName->ToElement();
		string strObjType;
		s16 nObjType = 0;
		if (eleConfigFileName->Attribute("typename")) {
			strObjType = eleConfigFileName->Attribute("typename");
		}
		if (eleConfigFileName->Attribute("typeid")) {
			nObjType = atoi(eleConfigFileName->Attribute("typeid"));
		}
		if (strObjType.empty() || ++nObjTypeIndex != nObjType) {
			return false;
		}
		vObjTypes.push_back(strObjType);
		m_mapObjType[strObjType] = nObjType;
	}

	for (vector<string>::iterator itr = vObjTypes.begin(); itr != vObjTypes.end(); ++itr) {
		if (!LoadConfig(*itr)) {
			m_mapAttrConfig = mapAttrConfig;
			return false;
		}
	}

	return true;
}

bool CAttributeMgr::LoadConfig(const string& objType)
{
	string objconfigfile = string(tools::GetAppPath()) + "/logic_config/ObjectMgr/" + objType + ".xml";

	TiXmlDocument doc(objconfigfile.c_str());
	if (!doc.LoadFile()) {
		return false;
	}

	TiXmlNode* ndRoot = doc.RootElement();
	if (!ndRoot) {
		return false;
	}

	TiXmlElement* eleRoot = ndRoot->ToElement();
	string strBaseType;
	if (eleRoot->Attribute("base"))
		strBaseType = eleRoot->Attribute("base");

	ATTR_CONFIG* pBaseConfig = GetAttrConfig(strBaseType);
	if (!pBaseConfig && !strBaseType.empty())
		return false;
	m_mapAttrConfig[objType].strBaseType = strBaseType;

	size_t nStartPos = 0;
	for (TiXmlNode* ndAttr = ndRoot->FirstChild("attr"); ndAttr; ndAttr = ndAttr->NextSibling("attr")) {
		TiXmlElement* eleAttr = ndAttr->ToElement();
		string strAttrName;
		attr_info info;
		if (eleAttr->Attribute("name"))
			strAttrName = eleAttr->Attribute("name");
		if (eleAttr->Attribute("type"))
			info.type = GetType(eleAttr->Attribute("type"));

		switch (info.type)
		{
		case DATA_TYPE_S8:
			info.length = sizeof(s8);
			break;
		case DATA_TYPE_S16:
			info.length = sizeof(s16);
			break;
		case DATA_TYPE_S32:
			info.length = sizeof(s32);
			break;
		case DATA_TYPE_S64:
			info.length = sizeof(s64);
			break;
		case DATA_TYPE_STRING:
		case DATA_TYPE_BLOB:
			{
				if (eleAttr->Attribute("length"))
					info.length = atoi(eleAttr->Attribute("length"));
				if (info.length == 0)
					return false;
			}
			break;
		default:
			{
				TASSERT(false, "format error");
				return false;
			}
			break;
		}

		info.seripos = nStartPos;
		nStartPos += info.length;

		m_mapAttrConfig[objType].mapAttrInfo[strAttrName] = info;
	}
	m_mapAttrConfig[objType].nBuffLen = nStartPos;
	return true;
}

s8 CAttributeMgr::GetType(const string& strType)
{
	return CData::GetDataType(strType.c_str());
}

attr_info CAttributeMgr::GetAttrInfo(const string& objName, const string& attrName)
{
	return m_mapAttrConfig[objName].mapAttrInfo[attrName];
}

ATTR_CONFIG* CAttributeMgr::GetAttrConfig(const string& objtype)
{
	MAP_ATTR_CONFIG::iterator itr = m_mapAttrConfig.find(objtype.c_str());
	if (itr != m_mapAttrConfig.end())
	{
		return &(itr->second);
	}
	return NULL;
}

s16 CAttributeMgr::GetObjType(const string& strObjType)
{
	MAP_OBJ_TYPE::iterator itr = m_mapObjType.find(strObjType);

	if (itr != m_mapObjType.end())
	{
		return itr->second;
	}
	return 0;
}