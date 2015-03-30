#include "Object.h"
#include "CData.h"
#include "CArgs.h"


OHandler ObjectIDFactory::FetchID(s16 nType)
{
	return OHandler(nType, ++m_nId);
}

bool operator<(const OHandler& lhs, const OHandler& rhs)
{
	return lhs.objectID < rhs.objectID;
}

bool Object::SetType(const char * pType) {
	m_strType = pType;
	return Init(pType);
}

const char* Object::GetType(void) {
	return m_strType.c_str();
}

bool Object::CreateTable(const char* szTableName, const vector<string>& vecColumnTypes) {
	if (m_mapTables.find(szTableName) != m_mapTables.end())
		return false;

	Table tbl;
	if (!tbl.Init(vecColumnTypes)) {
		return false;
	}
	m_mapTables[szTableName] = tbl;
	return true;
}

bool Object::DestroyTable(const char* szTableName) {
	if (m_mapTables.find(szTableName) == m_mapTables.end())
		return false;

	m_mapTables.erase(szTableName);
	return true;
}

bool Object::InsertRow(const char* szTableName, CArgs& args) {
	if (m_mapTables.find(szTableName) == m_mapTables.end())
		return false;
	return m_mapTables[szTableName].InsertRow(args);
}

bool Object::DeleteRow(const char* szTableName, size_t nRow) {
	if (m_mapTables.find(szTableName) == m_mapTables.end())
		return false;
	return m_mapTables[szTableName].DeleteRow(nRow);
}

bool Object::SetTableValue(const char* szTableName, size_t nRow, size_t nCol, const char* szValue, size_t nLen) {
	if (m_mapTables.find(szTableName) == m_mapTables.end())
		return false;
	return m_mapTables[szTableName].SetValue(nRow, nCol, szValue, nLen);
}

bool Object::GetTableValue(const char* szTableName, size_t nRow, size_t nCol, string& strValue) {
	if (m_mapTables.find(szTableName) == m_mapTables.end())
		return false;
	return m_mapTables[szTableName].GetValue(nRow, nCol, strValue);
}

Object::Table::Table(void)
	: m_nRowBufLen(0)
{}

bool Object::Table::Init(const TABLE_COLUMN_TYPES& vecColumnTypes) {
	m_mapStartPos.clear();
	m_rows.clear();
	m_nRowBufLen = 0;
	m_mapColLen.clear();
	if (vecColumnTypes.empty()) {
		return false;
	}
	size_t nColIndex = 0;
	size_t nStartPosIndex = 0;
	for (TABLE_COLUMN_TYPES::const_iterator citr = vecColumnTypes.begin(); citr != vecColumnTypes.end(); ++citr) {
		s8 datatype = CData::GetDataType(citr->c_str());
		switch (datatype) {
		case DATA_TYPE_S8:
		case DATA_TYPE_S16:
		case DATA_TYPE_S32:
		case DATA_TYPE_S64:
		case DATA_TYPE_DOUBLE:
			m_mapStartPos[nColIndex] = nStartPosIndex;
			m_mapColLen[nColIndex] = CData::GetDataLength(datatype);
			nStartPosIndex += CData::GetDataLength(datatype);
			break;
		case DATA_TYPE_STRING:
			m_mapStartPos[nColIndex] = nStartPosIndex;
			m_mapColLen[nColIndex] = MAX_TABLE_DATA_LENGTH;
			nStartPosIndex += MAX_TABLE_DATA_LENGTH;
			break;
		default:
			return false;
			break;
		}
		++nColIndex;
	}
	m_nRowBufLen = nStartPosIndex;
	return true;
}

size_t Object::Table::GetRowCount(void) {
	return m_rows.size();
}

size_t Object::Table::GetColumnCount(void) {
	return m_mapStartPos.size();
}

bool Object::Table::InsertRow(CArgs& args) {
	if (GetColumnCount() != args.GetDataNum()) {
		return false;
	}

	size_t nArgIndex = 0;
	TableRow row;
	row.pBlob = NEW char[m_nRowBufLen];
	if (!row.pBlob) {
		return false;
	}
	row.nBlobLen = m_nRowBufLen;
	while (!args.IsEmpty()) {
		CData data;
		args>>data;
		size_t datalen = (data.type == DATA_TYPE_STRING) ? MAX_TABLE_DATA_LENGTH : CData::GetDataLength(data.type);
		if (m_mapColLen[nArgIndex] != datalen) {
			delete [] row.pBlob;
			return false;
		}
		data.SetDataToMem(row.pBlob + m_mapStartPos[nArgIndex], datalen);
		++nArgIndex;
	}

	m_rows.push_back(row);
	return true;
}

bool Object::Table::DeleteRow(size_t nRow) {
	if (GetRowCount() <= nRow) {
		return false;
	}
	vector<TableRow>::iterator itDelete = m_rows.begin() + nRow;
	m_rows.erase(itDelete);
	return true;
}

bool Object::Table::SetValue(size_t nRow, size_t nCol, const char* szValue, size_t nLen) {
	if (MAX_TABLE_DATA_LENGTH < nLen + 1) {
		return false;
	}
	if (GetColumnCount() <= nCol || GetRowCount() <= nRow) {
		return false;
	}
	size_t nStartPos = m_mapStartPos[nCol];
	size_t nColLen = m_mapColLen[nCol];
	if (nColLen != MAX_TABLE_DATA_LENGTH) {
		return false;
	}
	TableRow& row = m_rows[nRow];
	memset(row.pBlob + nStartPos, 0, MAX_TABLE_DATA_LENGTH);
	tools::SafeMemcpy(row.pBlob + nStartPos, MAX_TABLE_DATA_LENGTH, szValue, nLen);
	return true;
}

bool Object::Table::GetValue(size_t nRow, size_t nCol, string& strValue) {
	if (GetColumnCount() <= nCol || GetRowCount() <= nRow) {
		return false;
	}
	size_t nStartPos = m_mapStartPos[nCol];
	size_t nColLen = m_mapColLen[nCol];
	if (nColLen != MAX_TABLE_DATA_LENGTH) {
		return false;
	}
	TableRow& row = m_rows[nRow];
	const char* szValue = row.pBlob + nStartPos;
	strValue = string(szValue, strlen(szValue));
	return true;
}
