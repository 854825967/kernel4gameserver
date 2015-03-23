#include "Attrib.h"
#include "tinyxml/tinyxml.h"
#include "CData.h"

#define CHECK_ATTR_LENGTH_SET(attrname, attrlen)				\
	if (!IsValidAttrLength(attrname, attrlen, true))			\
	{															\
		return false;											\
	}															\

#define CHECK_ATTR_LENGTH_GET(attrname, attrlen)				\
	if (!IsValidAttrLength(attrname, attrlen, false))			\
	{															\
		return false;											\
	}															\



Attrib::Attrib(void)
	: m_pMapAttrInfo(NULL)
	, m_pAttribBase(NULL)
	, m_pBuf(NULL)
	, m_nBufLen(0)
{
}

Attrib::~Attrib(void) {
	if (m_pBuf) {
		delete [] m_pBuf;
	}
	if (m_pAttribBase) {
		delete m_pAttribBase;
	}
}

bool Attrib::Init(const char* name)
{
	ATTR_CONFIG* pAttrConfig = CAttributeMgr::Instance()->GetAttrConfig(name);

	if (!pAttrConfig)
		return false;

	m_pMapAttrInfo = &pAttrConfig->mapAttrInfo;
	m_nBufLen = pAttrConfig->nBuffLen;
	m_pBuf = NEW char[m_nBufLen];
	if (!m_pBuf)
		return false;
	memset(m_pBuf, 0, m_nBufLen);

	if (!pAttrConfig->strBaseType.empty()) {
		Attrib* pAttribBase = NEW Attrib;
		if (!pAttribBase->Init(pAttrConfig->strBaseType.c_str())) {
			delete pAttribBase;
			return false;
		}
		m_pAttribBase = pAttribBase;
	}
	return true;
}

MAP_ATTR_INFO* Attrib::FindAttrInfoMap(const char* attrname) {
	if (m_pMapAttrInfo->find(attrname) != m_pMapAttrInfo->end())
		return m_pMapAttrInfo;

	if (!m_pAttribBase) {
		return NULL;
	}

	return m_pAttribBase->FindAttrInfoMap(attrname);
}

bool Attrib::IsValidAttrLength(const char* attrname, size_t attrlength, bool bIsSet)
{
	MAP_ATTR_INFO* pAttrInfoMap = FindAttrInfoMap(attrname);
	if (!pAttrInfoMap)
		return false;

	MAP_ATTR_INFO::iterator itr = pAttrInfoMap->find(attrname);
	
	if (itr != pAttrInfoMap->end())
	{
		switch(itr->second.type)
		{
		case DATA_TYPE_S8:
		case DATA_TYPE_S16:
		case DATA_TYPE_S32:
		case DATA_TYPE_S64:
			return (attrlength == itr->second.length);
		case DATA_TYPE_STRING:
		case DATA_TYPE_BLOB:
			return bIsSet ? (attrlength <= itr->second.length) : (attrlength >= itr->second.length);
		default:
			return false;
		}
	}
	return false;
}

bool Attrib::SetAttr(const char* szAttrName, const char* szAttrValue, size_t nLen) {
	return SetValue(szAttrName, szAttrValue, nLen + 1);
}

bool Attrib::GetAttr(const char* szAttrName, string& strAttrValue) {
	char szAttrValue [MAX_ATTR_STRING_LEN] = {0};
	if (!GetValue(szAttrName, szAttrValue, MAX_ATTR_STRING_LEN))
		return false;

	strAttrValue = string(szAttrValue, strlen(szAttrValue));
	return true;
}

bool Attrib::SetAttr(const char* szAttrName, const void* pBlob, size_t nBlobLen) {
	if (nBlobLen)
	{
		return SetValue(szAttrName, pBlob, nBlobLen);
	}
	return false;
}

bool Attrib::GetAttr(const char* szAttrName, void* pBlob, size_t nBlobLen) {
	if (nBlobLen)
	{
		return GetValue(szAttrName, pBlob, nBlobLen);
	}
	return false;
}

s8 Attrib::GetAttrType(const char* szAttrName) {
	MAP_ATTR_INFO* pAttrInfoMap = FindAttrInfoMap(szAttrName);
	if (!pAttrInfoMap)
		return DATA_TYPE_INVALID;

	MAP_ATTR_INFO::iterator itr = pAttrInfoMap->find(szAttrName);
	return itr->second.type;
}

size_t Attrib::GetAttrLength(const char* szAttrName) {
	MAP_ATTR_INFO* pAttrInfoMap = FindAttrInfoMap(szAttrName);
	if (!pAttrInfoMap)
		return 0;

	MAP_ATTR_INFO::iterator itr = pAttrInfoMap->find(szAttrName);
	return itr->second.length;
}

bool Attrib::SetValue(const char* szAttrName, const void* pValue, size_t nLen) {
	CHECK_ATTR_LENGTH_SET(szAttrName, nLen);
	if (m_pMapAttrInfo) {
		MAP_ATTR_INFO::iterator itr = m_pMapAttrInfo->find(szAttrName);
		if (itr != m_pMapAttrInfo->end()) {
			const attr_info& info = itr->second;
			memset(m_pBuf + info.seripos, 0, info.length);
			memcpy(m_pBuf + info.seripos, pValue, nLen);
			return true;
		}
	}
	
	if (!m_pAttribBase) {
		return false;
	}
	return m_pAttribBase->SetValue(szAttrName, pValue, nLen);
}

bool Attrib::GetValue(const char* szAttrName, void* pValue, size_t nLen) {
	CHECK_ATTR_LENGTH_GET(szAttrName, nLen);
	if (m_pMapAttrInfo) {
		MAP_ATTR_INFO::iterator itr = m_pMapAttrInfo->find(szAttrName);
		if (itr != m_pMapAttrInfo->end()) {
			const attr_info& info = itr->second;
			memcpy(pValue, m_pBuf + info.seripos, info.length);
			return true;
		}
	}
	if (!m_pAttribBase) {
		return false;
	}
	return m_pAttribBase->GetValue(szAttrName, pValue, nLen);
}

void Attrib::GetAllAttrs(vector<string>& vAttrs) {
	GetSelfAttrs(vAttrs);
	if (m_pAttribBase) {
		m_pAttribBase->GetAllAttrs(vAttrs);
	}
}

void Attrib::GetSelfAttrs(vector<string>& vAttrs) {
	if (m_pMapAttrInfo)	{
		for (MAP_ATTR_INFO::iterator itr = m_pMapAttrInfo->begin(); itr != m_pMapAttrInfo->end(); ++itr) {
			vAttrs.push_back(itr->first);
		}
	}
}