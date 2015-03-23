#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "Attrib.h"
#include "IObjectMgr.h"
#include "Tools.h"

#define MAX_TABLE_DATA_LENGTH	64

union objecthandle {
	objecthandle(s16 typeB, s32 objID)
	{
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

bool operator<(const objecthandle& lhs, const objecthandle& rhs);

class ObjectIDFactory {
public:
	static ObjectIDFactory* getInstance(void)
	{
		static ObjectIDFactory* pIns = NULL;
		if (pIns == NULL)
		{
			pIns = NEW ObjectIDFactory;
		}
		return pIns;
	}
	ObjectIDFactory()
		: m_nId (0)
	{}
	objecthandle FetchID (s16 nType);
private:
	s32 m_nId;
};


class CArgs;

class Object : public Attrib {
public:
	bool SetType(const char* pType);
	const char* GetType(void);

	bool CreateTable(const char* szTableName, const vector<string>& vecColumnTypes);
	bool DestroyTable(const char* szTableName);

	bool InsertRow(const char* szTableName, CArgs& args);
	bool DeleteRow(const char* szTableName, size_t nRow);

	template<typename IntType>
	bool SetTableValue(const char* szTableName, size_t nRow, size_t nCol, IntType nValue) {
		if (m_mapTables.find(szTableName) == m_mapTables.end())
			return false;
		return m_mapTables[szTableName].SetValue(nRow, nCol, nValue);
	}
	template<typename IntType>
	bool GetTableValue(const char* szTableName, size_t nRow, size_t nCol, IntType& nValue) {
		if (m_mapTables.find(szTableName) == m_mapTables.end())
			return false;
		return m_mapTables[szTableName].GetValue(nRow, nCol, nValue);
	}

	bool SetTableValue(const char* szTableName, size_t nRow, size_t nCol, const char* szValue, size_t nLen);
	bool GetTableValue(const char* szTableName, size_t nRow, size_t nCol, string& strValue);
	
protected:
	struct TableRow {
		TableRow()
			: pBlob (NULL)
			, nBlobLen (0)
		{}
		char* pBlob;
		size_t nBlobLen;
	};

	class Table {
	public:
		Table(void);
		bool Init(const TABLE_COLUMN_TYPES& vecColumnTypes);
		size_t GetRowCount(void);
		size_t GetColumnCount(void);
		bool InsertRow(CArgs& args);
		bool DeleteRow(size_t nRow);
		
		template<typename IntType>
		bool SetValue(size_t nRow, size_t nCol, IntType nValue);
		template<typename IntType>
		bool GetValue(size_t nRow, size_t nCol, IntType& nValue);

		bool SetValue(size_t nRow, size_t nCol, const char* szValue, size_t nLen);
		bool GetValue(size_t nRow, size_t nCol, string& strValue);
	private:
		vector<TableRow> m_rows;
		map<size_t, size_t> m_mapStartPos;
		map<size_t, size_t> m_mapColLen;
		size_t m_nRowBufLen;
	};

	map<string, Table> m_mapTables;
	string m_strType;
};

template<typename IntType>
bool Object::Table::SetValue(size_t nRow, size_t nCol, IntType nValue) {
	if (GetColumnCount() <= nCol || GetRowCount() <= nRow) {
		return false;
	}
	size_t nStartPos = m_mapStartPos[nCol];
	size_t nColLen = m_mapColLen[nCol];
	if (nColLen != sizeof(IntType)) {
		return false;
	}
	TableRow& row = m_rows[nRow];
	tools::SafeMemcpy(row.pBlob + nStartPos, row.nBlobLen - nStartPos, &nValue, sizeof(IntType));
	return true;
}

template<typename IntType>
bool Object::Table::GetValue(size_t nRow, size_t nCol, IntType& nValue) {
	if (GetColumnCount() <= nCol || GetRowCount() <= nRow) {
		return false;
	}
	size_t nStartPos = m_mapStartPos[nCol];
	size_t nColLen = m_mapColLen[nCol];
	if (nColLen != sizeof(IntType)) {
		return false;
	}
	TableRow& row = m_rows[nRow];
	tools::SafeMemcpy(&nValue, nColLen, row.pBlob + nStartPos, nColLen);
	return true;
}


#endif