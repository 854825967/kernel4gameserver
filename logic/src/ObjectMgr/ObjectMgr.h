#ifndef __OBJECTMGR_H__
#define __OBJECTMGR_H__

#include "IObjectMgr.h"
#include "Object.h"
#include <map>
using namespace std;

union objecthandle;


class ObjectMgr : public IObjectMgr
{
public:
	virtual bool Initialize(IKernel * pKernel);
	virtual bool Launched(IKernel * pKernel);
	virtual bool Destroy(IKernel * pKernel);
	
	virtual objecthandle CreateObject(const string& strObjectType);
	virtual bool CreateObject(const string& strObjectType, const objecthandle& objId);
	virtual bool DestroyObject(const objecthandle& hd);
	virtual bool Exists(const objecthandle& hd);
	
	virtual bool SetAttr(const objecthandle& hd, const char* szAttrName, s8 nValue);
	virtual bool GetAttr(const objecthandle& hd, const char* szAttrName, s8& nValue);

	virtual bool SetAttr(const objecthandle& hd, const char* szAttrName, s16 nValue);
	virtual bool GetAttr(const objecthandle& hd, const char* szAttrName, s16& nValue);

	virtual bool SetAttr(const objecthandle& hd, const char* szAttrName, s32 nValue);
	virtual bool GetAttr(const objecthandle& hd, const char* szAttrName, s32& nValue);

	virtual bool SetAttr(const objecthandle& hd, const char* szAttrName, s64 nValue);
	virtual bool GetAttr(const objecthandle& hd, const char* szAttrName, s64& nValue);

	virtual bool SetAttr(const objecthandle& hd, const char* szAttrName, double dValue);
	virtual bool GetAttr(const objecthandle& hd, const char* szAttrName, double& dValue);

	virtual bool SetAttr(const objecthandle& hd, const char* szAttrName, const char* szAttrValue, size_t nLen);
	virtual bool GetAttr(const objecthandle& hd, const char* szAttrName, string& strValue);

	virtual bool SetAttr(const objecthandle& hd, const char* szAttrName, const void* pBlob, size_t nBlobLen);
	virtual bool GetAttr(const objecthandle& hd, const char* szAttrName, void* pBlob, size_t nBlobLen);

	virtual bool CreateTable(const objecthandle& hd, const char* szTableName, const TABLE_COLUMN_TYPES& coltypes);
	virtual bool DestroyTable(const objecthandle& hd, const char* szTableName);

	virtual bool InsertTableRow(const objecthandle& hd, const char* szTableName, CArgs& args);
	virtual bool DeleteTableRow(const objecthandle& hd, const char* szTableName, size_t nRow);

	template<typename IntType>
	bool SetTableValue(const objecthandle& hd, const char* szTableName, size_t nRow, size_t nCol, IntType nValue);
	template<typename IntType>
	bool GetTableValue(const objecthandle& hd, const char* szTableName, size_t nRow, size_t nCol, IntType& nValue);

	virtual bool SetTableValue(const objecthandle& hd, const char* szTableName, size_t nRow, size_t nCol, const char* szValue, size_t nLen);
	virtual bool GetTableValue(const objecthandle& hd, const char* szTableName, size_t nRow, size_t nCol, string& strValue);

	typedef void (*SET_ATTR_CALLBACK)(void);
	void RegisterSetAttrCallback(const char* szObjType, const char* szAttrName, SET_ATTR_CALLBACK callback);

private:
	s16 GetObjType(const string& strObjType);
	s8 GetAttrType(const objecthandle& objhd, const char* szAttrName);
	size_t GetAttrLength(const objecthandle& objhd, const char* szAttrName);
	void GetAllAttrs(const objecthandle& objhd, vector<string>& vAttrs);
	void GetSelfAttrs(const objecthandle& objhd, vector<string>& vAttrs);
	Object* FindObject(const objecthandle& objh);
	void call_back(const char* szObjType, const char* szAttrName);
	map<string, map<string, SET_ATTR_CALLBACK> > m_mapSetAttrCallBack;
	map<s64, Object*> m_mapObject;
};



#endif