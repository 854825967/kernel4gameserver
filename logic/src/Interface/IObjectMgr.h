/* 
 * File:   IObjectMgr.h
 * Author: alax
 *
 * Created on March 3, 2015, 10:46 AM
 */

#ifndef __IObjectMgr_h__
#define	__IObjectMgr_h__

#include "IModule.h"
#include <vector>

using namespace std;


union OHandler {
    OHandler(s16 typeB, s32 objID) {
        sType.typeA = 0;
        sType.typeB = typeB;
        sType.ID = objID;
    }

    struct {
        s16 typeA;
        s16 typeB;
        s32 ID;
    }sType;
    s64 objectID;
};

class CArgs;

typedef vector<string> TABLE_COLUMN_TYPES;

class IObjectMgr : public IModule {
public:
	virtual OHandler CreateObject(const string& strObjectType) = 0;
	virtual bool CreateObject(const string& strObjectType, const OHandler& objId) = 0;
	virtual bool DestroyObject(const OHandler& hd) = 0;
	virtual bool Exists(const OHandler& hd) = 0;

	virtual bool SetAttr(const OHandler& hd, const char* szAttrName, s8 nValue) = 0;
	virtual bool GetAttr(const OHandler& hd, const char* szAttrName, s8& nValue) = 0;

	virtual bool SetAttr(const OHandler& hd, const char* szAttrName, s16 nValue) = 0;
	virtual bool GetAttr(const OHandler& hd, const char* szAttrName, s16& nValue) = 0;

	virtual bool SetAttr(const OHandler& hd, const char* szAttrName, s32 nValue) = 0;
	virtual bool GetAttr(const OHandler& hd, const char* szAttrName, s32& nValue) = 0;

	virtual bool SetAttr(const OHandler& hd, const char* szAttrName, s64 nValue) = 0;
	virtual bool GetAttr(const OHandler& hd, const char* szAttrName, s64& nValue) = 0;

	virtual bool SetAttr(const OHandler& hd, const char* szAttrName, double dValue) = 0;
	virtual bool GetAttr(const OHandler& hd, const char* szAttrName, double& dValue) = 0;

	virtual bool SetAttr(const OHandler& hd, const char* szAttrName, const char* szAttrValue, size_t nLen) = 0;
	virtual bool GetAttr(const OHandler& hd, const char* szAttrName, string& strValue) = 0;

	virtual bool SetAttr(const OHandler& hd, const char* szAttrName, const void* pBlob, size_t nBlobLen) = 0;
	virtual bool GetAttr(const OHandler& hd, const char* szAttrName, void* pBlob, size_t nBlobLen) = 0;

	virtual bool CreateTable(const OHandler& hd, const char* szTableName, const TABLE_COLUMN_TYPES& coltypes) = 0;
	virtual bool DestroyTable(const OHandler& hd, const char* szTableName) = 0;

	virtual bool InsertTableRow(const OHandler& hd, const char* szTableName, CArgs& args) = 0;
	virtual bool DeleteTableRow(const OHandler& hd, const char* szTableName, size_t nRow) = 0;

	virtual bool SetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s8 nValue) = 0;
	virtual bool GetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s8& nValue) = 0;

	virtual bool SetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s16 nValue) = 0;
	virtual bool GetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s16& nValue) = 0;

	virtual bool SetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s32 nValue) = 0;
	virtual bool GetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s32& nValue) = 0;

	virtual bool SetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s64 nValue) = 0;
	virtual bool GetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, s64& nValue) = 0;

	virtual bool SetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, double dValue) = 0;
	virtual bool GetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, double& dValue) = 0;

	virtual bool SetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, const char* szValue, size_t nLen) = 0;
	virtual bool GetTableValue(const OHandler& hd, const char* szTableName, size_t nRow, size_t nCol, string& strValue) = 0;
};

#endif	/* __IObjectMgr_h__ */

