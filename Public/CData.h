#ifndef __CData_h__
#define __CData_h__

#include "MultiSys.h"
#include "Tools.h"
#include <map>

using namespace std;

#define MAX_DATA_STRING_LEN 64

enum DATA_TYPE {
	DATA_TYPE_INVALID,
	DATA_TYPE_S8,
	DATA_TYPE_S16,
	DATA_TYPE_S32,
	DATA_TYPE_S64,
	DATA_TYPE_DOUBLE,
	DATA_TYPE_STRING,
	DATA_TYPE_BLOB
};

class CData {
public:
	CData()
		: type(DATA_TYPE_INVALID)
	{}
	~CData()
	{
		if (type == DATA_TYPE_STRING && strValue) {
			delete strValue;
		}
	}
	union {
		s8 s8Value;
		s16 s16Value;
		s32 s32Value;
		s64 s64Value;
		double dValue;
		string* strValue;
	};
	s8 type;

	static s8 GetDataType(const char* szDataTypeName) {
		static map<string, s8> mapDataType;
		if (mapDataType.empty()) {
			mapDataType["s8"] = DATA_TYPE_S8;
			mapDataType["s16"] = DATA_TYPE_S16;
			mapDataType["s32"] = DATA_TYPE_S32;
			mapDataType["s64"] = DATA_TYPE_S64;
			mapDataType["double"] = DATA_TYPE_DOUBLE;
			mapDataType["string"] = DATA_TYPE_STRING;
			mapDataType["blob"] = DATA_TYPE_BLOB;
		}
		if (mapDataType.find(szDataTypeName) != mapDataType.end()) {
			return mapDataType[szDataTypeName];
		}
		return DATA_TYPE_INVALID;
	}

	static s8 GetDataType(s8 value) {
		return DATA_TYPE_S8;
	}

	static s8 GetDataType(s16 value) {
		return DATA_TYPE_S16;
	}

	static s8 GetDataType(s32 value) {
		return DATA_TYPE_S32;
	}

	static s8 GetDataType(s64 value) {
		return DATA_TYPE_S64;
	}

	static s8 GetDataType(double value) {
		return DATA_TYPE_DOUBLE;
	}

	static size_t GetDataLength(s8 type) {
		switch (type)
		{
		case DATA_TYPE_S8:
			return sizeof(s8);
			break;
		case DATA_TYPE_S16:
			return sizeof(s16);
			break;
		case DATA_TYPE_S32:
			return sizeof(s32);
			break;
		case DATA_TYPE_S64:
			return sizeof(s64);
			break;
		case DATA_TYPE_DOUBLE:
			return sizeof(double);
			break;
		case DATA_TYPE_STRING:
			break;
		default:
			break;
		}
		return 0;
	}

	void SetDataToMem(void* pMem, size_t nLen) {
		memset(pMem, 0, nLen);
		switch (type)
		{
		case DATA_TYPE_S8:
			tools::SafeMemcpy(pMem, nLen, &s8Value, sizeof(s8));
			break;
		case DATA_TYPE_S16:
			tools::SafeMemcpy(pMem, nLen, &s16Value, sizeof(s16));
			break;
		case DATA_TYPE_S32:
			tools::SafeMemcpy(pMem, nLen, &s32Value, sizeof(s32));
			break;
		case DATA_TYPE_S64:
			tools::SafeMemcpy(pMem, nLen, &s64Value, sizeof(s64));
			break;
		case DATA_TYPE_DOUBLE:
			tools::SafeMemcpy(pMem, nLen, &dValue, sizeof(double));
			break;
		case DATA_TYPE_STRING:
			tools::SafeMemcpy(pMem, nLen, strValue->c_str(), strValue->length());
			break;
		default:
			break;
		}
	}

	size_t GetDataSize(void) const{
		switch (type)
		{
		case DATA_TYPE_S8:
		case DATA_TYPE_S16:
		case DATA_TYPE_S32:
		case DATA_TYPE_S64:
		case DATA_TYPE_DOUBLE:
			return GetDataLength(type) + sizeof(s8);
			break;
		case DATA_TYPE_STRING:
			return sizeof(size_t) + strValue->length() + sizeof(s8);
			break;
		default:
			break;
		}
		return 0;
	}

	void SetString(const char* szStrValue) {
		this->strValue = NEW string(szStrValue, strlen(szStrValue));
		this->type = DATA_TYPE_STRING;
	}
	void GetString(string& strValue) {
		if (type == DATA_TYPE_STRING) {
			strValue = string(this->strValue->c_str(), this->strValue->length());
		}
	}

	void SetS8(s8 s8Value) {
		this->s8Value = s8Value;
		this->type = DATA_TYPE_S8;
	}
	void GetS8(s8& s8Value) {
		if (type == DATA_TYPE_S8) {
			s8Value = this->s8Value;
		}
	}

	void SetS16(s16 s16Value) {
		this->s16Value = s16Value;
		this->type = DATA_TYPE_S16;
	}
	void GetS16(s16& s16Value) {
		if (type == DATA_TYPE_S16) {
			s16Value = this->s16Value;
		}
	}

	void SetS32(s32 s32Value) {
		this->s32Value = s32Value;
		this->type = DATA_TYPE_S32;
	}
	void GetS32(s32& s16Value) {
		if (type == DATA_TYPE_S32) {
			s32Value = this->s32Value;
		}
	}

	void SetS64(s64 s64Value) {
		this->s64Value = s64Value;
		this->type = DATA_TYPE_S64;
	}
	void GetS64(s64& s64Value) {
		if (type == DATA_TYPE_S64) {
			s64Value = this->s64Value;
		}
	}

	void SetDouble(double dValue) {
		this->dValue = dValue;
		this->type = DATA_TYPE_DOUBLE;
	}
	void GetDouble(double& dValue) {
		if (type == DATA_TYPE_DOUBLE) {
			dValue = this->dValue;
		}
	}

	template<typename Type>
	void SetValue(Type value) {
		switch (type) {
		case DATA_TYPE_S8:
			s8Value = value;
			break;
		case DATA_TYPE_S16:
			s16Value = value;
			break;
		case DATA_TYPE_S32:
			s32Value = value;
			break;
		case DATA_TYPE_S64:
			s64Value = value;
			break;
		case DATA_TYPE_DOUBLE:
			dValue = value;
			break;
		}
	}
};

#endif // __CData_h__
