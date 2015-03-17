#include "Attrib.h"
#include "tinyxml/tinyxml.h"

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



CAttrib::CAttrib()
{
	m_pAttrConfig = NULL;
	memset(m_cBuf, 0, sizeof(m_cBuf));
}

bool CAttrib::Init(const char * name)
{
	return (m_pAttrConfig = CAttributeMgr::Instance()->GetAttrConfig(name)) != NULL;
}

bool CAttrib::IsValidAttrLength(const char* attrname, s32 attrlength, bool bIsSet)
{
	MAP_ATTR::iterator itr = m_pAttrConfig->find(attrname);
	
	if (itr != m_pAttrConfig->end())
	{
		switch(itr->second.type)
		{
		case ATTR_TYPE_S8:
		case ATTR_TYPE_S16:
		case ATTR_TYPE_S32:
		case ATTR_TYPE_S64:
			return (attrlength == itr->second.length);
		case ATTR_TYPE_STRING:
		case ATTR_TYPE_BLOB:
			return bIsSet ? (attrlength <= itr->second.length) : (attrlength >= itr->second.length);
		default:
			return false;
		}
	}
	return false;
}

bool CAttrib::SetAttr(const char* szAttrName, s8 nNum)
{
	return SetValue(szAttrName, &nNum, sizeof(s8));
}

bool CAttrib::GetAttr(const char* szAttrName, s8& nNum)
{
	return GetValue(szAttrName, &nNum, sizeof(s8));
}

bool CAttrib::SetAttr(const char* szAttrName, s16 nNum)
{
	return SetValue(szAttrName, &nNum, sizeof(s16));
}

bool CAttrib::GetAttr(const char* szAttrName, s16& nNum)
{
	return GetValue(szAttrName, &nNum, sizeof(s16));
}

bool CAttrib::SetAttr(const char* szAttrName, s32 nNum)
{
	return SetValue(szAttrName, &nNum, sizeof(s32));
}

bool CAttrib::GetAttr(const char* szAttrName, s32& nNum)
{
	return GetValue(szAttrName, &nNum, sizeof(s32));
}

bool CAttrib::SetAttr(const char* szAttrName, s64 nNum)
{
	return SetValue(szAttrName, &nNum, sizeof(s64));
}

bool CAttrib::GetAttr(const char* szAttrName, s64& nNum)
{
	return GetValue(szAttrName, &nNum, sizeof(s64));
}

bool CAttrib::SetAttr(const char* szAttrName, const char* szAttrValue, s32 nLen)
{
	return SetValue(szAttrName, szAttrValue, nLen + 1);
}

bool CAttrib::GetAttr(const char* szAttrName, char* szAttrValue, s32 nLen)
{
	return GetValue(szAttrName, (void*)szAttrValue, nLen);
}

bool CAttrib::SetAttr(const char* szAttrName, const void* pBlob, s32 nBlobLen)
{
	if (nBlobLen)
	{
		return SetValue(szAttrName, pBlob, nBlobLen);
	}
	return false;
}

bool CAttrib::GetAttr(const char* szAttrName, void* pBlob, s32 nBlobLen)
{
	if (nBlobLen)
	{
		return GetValue(szAttrName, pBlob, nBlobLen);
	}
	return false;
}

bool CAttrib::SetValue(const char* szAttrName, const void* pValue, s32 nLen)
{
	CHECK_ATTR_LENGTH_SET(szAttrName, nLen);
	MAP_ATTR::iterator itr = m_pAttrConfig->find(szAttrName);
	if (itr != m_pAttrConfig->end())
	{
		const attr_info& info = itr->second;
		memset(m_cBuf + info.seripos, 0, info.length);
		memcpy(m_cBuf + info.seripos, pValue, nLen);
		return true;
	}
	return false;
}

bool CAttrib::GetValue(const char* szAttrName, void* pValue, s32 nLen)
{
	CHECK_ATTR_LENGTH_GET(szAttrName, nLen);
	MAP_ATTR::iterator itr = m_pAttrConfig->find(szAttrName);
	if (itr != m_pAttrConfig->end())
	{
		const attr_info& info = itr->second;
		memcpy(pValue, m_cBuf + info.seripos, info.length);
		return true;
	}
	return false;
}

s32 CAttrib::GetAttrLength(const char* szAttrName)
{
	MAP_ATTR::iterator itr = m_pAttrConfig->find(szAttrName);
	if (itr != m_pAttrConfig->end())
	{
		return itr->second.length;
	}
	return 0;
}