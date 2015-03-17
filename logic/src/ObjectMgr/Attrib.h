#ifndef __ATTRIB_H__
#define __ATTRIB_H__

#include <map>
#include <string>
#include "AttributeMgr.h"
using namespace std;


class CAttrib
{
public:
	CAttrib();
	
	bool SetAttr(const char* szAttrName, s8 nNum);
	bool GetAttr(const char* szAttrName, s8& nNum);
	bool SetAttr(const char* szAttrName, s16 nNum);
	bool GetAttr(const char* szAttrName, s16& nNum);
	bool SetAttr(const char* szAttrName, s32 nNum);
	bool GetAttr(const char* szAttrName, s32& nNum);
	bool SetAttr(const char* szAttrName, s64 nNum);
	bool GetAttr(const char* szAttrName, s64& nNum);
	bool SetAttr(const char* szAttrName, const char* szAttrValue, s32 nLen);
	bool GetAttr(const char* szAttrName, char* szAttrValue, s32 nLen);
	bool SetAttr(const char* szAttrName, const void* pBlob, s32 nBlobLen);
	bool GetAttr(const char* szAttrName, void* pBlob, s32 nBlobLen);

protected:
	bool Init(const char * name);

private:
	bool IsValidAttrLength(const char* attrname, s32 attrlength, bool bIsSet);
	bool SetValue(const char* szAttrName, const void* pValue, s32 nLen);
	bool GetValue(const char* szAttrName, void* pValue, s32 nLen);
	MAP_ATTR* m_pAttrConfig;
	s32 GetAttrLength(const char* szAttrName);
	char m_cBuf[MAX_ATTR_BUF_LEN];
};

#endif