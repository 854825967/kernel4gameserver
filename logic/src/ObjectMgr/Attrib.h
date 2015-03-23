#ifndef __ATTRIB_H__
#define __ATTRIB_H__

#include <map>
#include <vector>
#include <string>
#include "AttributeMgr.h"
using namespace std;


class Attrib
{
public:
	Attrib(void);
	~Attrib(void);
	

	template<typename IntType>
	bool SetAttr(const char* szAttrName, IntType nNum)
	{

		return SetValue(szAttrName, &nNum, sizeof(IntType));
	}
	template<typename IntType>
	bool GetAttr(const char* szAttrName, IntType& nNum)
	{

		return GetValue(szAttrName, &nNum, sizeof(IntType));
	}

	bool SetAttr(const char* szAttrName, const char* szAttrValue, size_t nLen);
	bool GetAttr(const char* szAttrName, string& strAttrValue);
	bool SetAttr(const char* szAttrName, const void* pBlob, size_t nBlobLen);
	bool GetAttr(const char* szAttrName, void* pBlob, size_t nBlobLen);
	s8 GetAttrType(const char* szAttrName);
	size_t GetAttrLength(const char* szAttrName);

	void GetAllAttrs(vector<string>& vAttrs);
	void GetSelfAttrs(vector<string>& vAttrs);
	
protected:
	bool Init(const char* name);

private:
	MAP_ATTR_INFO* FindAttrInfoMap(const char* attrname);
	bool IsValidAttrLength(const char* attrname, size_t attrlength, bool bIsSet);
	bool SetValue(const char* szAttrName, const void* pValue, size_t nLen);
	bool GetValue(const char* szAttrName, void* pValue, size_t nLen);

private:
	MAP_ATTR_INFO* m_pMapAttrInfo;
	Attrib* m_pAttribBase;
	char* m_pBuf;
	size_t m_nBufLen;
};

#endif