#ifndef __OBJECTMGR_H__
#define __OBJECTMGR_H__

#include "IObjectMgr.h"
#include "Object.h"
#include <map>
#include "TBundler.h"
using namespace std;


union OHandler;


class ObjectMgr : public IObjectMgr
{
public:
	virtual bool Initialize(IKernel * pKernel);
	virtual bool Launched(IKernel * pKernel);
	virtual bool Destroy(IKernel * pKernel);
	
	virtual OHandler CreateObject(const string& strObjectType);
	virtual bool CreateObject(const string& strObjectType, const OHandler& objId);
	virtual bool DestroyObject(const OHandler& hd);
	virtual bool Exists(const OHandler& hd);
	
	virtual bool SetAttr(const OHandler& hd, const char* szAttrName, s8 nValue);
	virtual bool GetAttr(const OHandler& hd, const char* szAttrName, s8& nValue);

	virtual bool SetAttr(const OHandler& hd, const char* szAttrName, s16 nValue);
	virtual bool GetAttr(const OHandler& hd, const char* szAttrName, s16& nValue);

	virtual bool SetAttr(const OHandler& hd, const char* szAttrName, s32 nValue);
	virtual bool GetAttr(const OHandler& hd, const char* szAttrName, s32& nValue);

	virtual bool SetAttr(const OHandler& hd, const char* szAttrName, s64 nValue);
	virtual bool GetAttr(const OHandler& hd, const char* szAttrName, s64& nValue);

	virtual bool SetAttr(const OHandler& hd, const char* szAttrName, double dValue);
	virtual bool GetAttr(const OHandler& hd, const char* szAttrName, double& dValue);

	virtual bool SetAttr(const OHandler& hd, const char* szAttrName, const char* szAttrValue, size_t nLen);
	virtual bool GetAttr(const OHandler& hd, const char* szAttrName, string& strValue);

	virtual bool SetAttr(const OHandler& hd, const char* szAttrName, const void* pBlob, size_t nBlobLen);
	virtual bool GetAttr(const OHandler& hd, const char* szAttrName, void* pBlob, size_t nBlobLen);

	virtual bool CreateTable(const OHandler& hd, const char* szTableName, const TABLE_COLUMN_TYPES& coltypes);
	virtual bool DestroyTable(const OHandler& hd, const char* szTableName);

	virtual bool InsertTableRow(const OHandler& hd, const char* szTableName, CArgs& args);
	virtual bool DeleteTableRow(const OHandler& hd, const char* szTableName, size_t nRow);

	virtual bool SetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s8 nValue);
	virtual bool GetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s8& nValue);

	virtual bool SetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s16 nValue);
	virtual bool GetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s16& nValue);

	virtual bool SetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s32 nValue);
	virtual bool GetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s32& nValue);

	virtual bool SetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s64 nValue);
	virtual bool GetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s64& nValue);

	virtual bool SetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, double dValue);
	virtual bool GetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, double& dValue);

	virtual bool SetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, const char* szValue, size_t nLen);
	virtual bool GetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, string& strValue);

	virtual bool RgsModifyAttributeCall(IModule* module, const string& szAttrName, mem_fun mf);
	virtual bool UnRgsModifyAttributeCall(IModule* module, const string& szAttrName, mem_fun mf);

private:
	s16 GetObjType(const string& strObjType);
	s8 GetAttrType(const OHandler& objhd, const char* szAttrName);
	size_t GetAttrLength(const OHandler& objhd, const char* szAttrName);
	void GetAllAttrs(const OHandler& objhd, vector<string>& vAttrs);
	void GetSelfAttrs(const OHandler& objhd, vector<string>& vAttrs);
	Object* FindObject(const OHandler& objh);

	template<typename Type>
	bool SetAttr(const OHandler& hd, const char* szAttrName, Type value) {
		Object* obj = FindObject(hd);
		if (obj && obj->SetAttr(szAttrName, value)) {
			CData data;
			data.type = CData::GetDataType(value);
			data.SetValue(value);
			m_callbacks.call_bundler(szAttrName, szAttrName, data);
			return true;
		}
		return false;
	}
	template<typename Type>
	bool GetAttr(const OHandler& hd, const char* szAttrName, Type& value) {
		Object* obj = FindObject(hd);
		if (obj && obj->GetAttr(szAttrName, value)) {
			return true;
		}
		return false;
	}

	template<typename Type>
	bool SetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, Type value) {
		Object* obj = FindObject(hd);
		if (!obj)
			return false;
		return obj->SetTableValue(szTableName, nRow, nCol, value);
	}

	template<typename Type>
	bool GetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, Type& value) {
		Object* obj = FindObject(hd);
		if (!obj)
			return false;
		return obj->GetTableValue(szTableName, nRow, nCol, value);
	}

private:
	map<s64, Object*> m_mapObject;
	tlib::bundler_pool<IModule, string, string, const CData&> m_callbacks;
};



#endif