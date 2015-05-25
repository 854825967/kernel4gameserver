#ifndef __CArgs_h__
#define __CArgs_h__


#include "MultiSys.h"
#include "CData.h"
#include "TStream.h"
#include "Tools.h"
#include <list>

using namespace std;

#define DEFAULT_BUF_LEN	1024

class CArgs : private tlib::TStream<DEFAULT_BUF_LEN, false>
{
public:
	CArgs()
		: m_nDataNum(0)
	{}

	void clear(void) {
		tlib::TStream<DEFAULT_BUF_LEN, false>::clear();
		m_nDataNum = 0;
	}

	CArgs& operator<<(const CData& data) {
		if (data.type < DATA_TYPE_S8 || data.type > DATA_TYPE_STRING) {
			return *this;
		}
		in(&data.type, sizeof(s8));
		switch (data.type)
		{
		case DATA_TYPE_S8:
			in(&data.s8Value, sizeof(s8));
			break;
		case DATA_TYPE_S16:
			in(&data.s16Value, sizeof(s16));
			break;
		case DATA_TYPE_S32:
			in(&data.s32Value, sizeof(s32));
			break;
		case DATA_TYPE_S64:
			in(&data.s64Value, sizeof(s64));
			break;
		case DATA_TYPE_DOUBLE:
			in(&data.dValue, sizeof(double));
			break;
		case DATA_TYPE_STRING:
			{
				size_t nStrLen = data.strValue->length();
				in(&nStrLen, sizeof(size_t));
				in(data.strValue->c_str(), nStrLen);
			}
			break;
		}
		++m_nDataNum;
		return *this;
	}

	CArgs& operator>>(CData& data) {
		data.type = DATA_TYPE_INVALID;
		tools::SafeMemcpy(&data.type, sizeof(s8), buff(), sizeof(s8));
		out(sizeof(s8));
		switch (data.type)
		{
		case DATA_TYPE_S8:
			tools::SafeMemcpy(&data.s8Value, sizeof(s8), buff(), sizeof(s8));
			out(sizeof(s8));
			break;
		case DATA_TYPE_S16:
			tools::SafeMemcpy(&data.s16Value, sizeof(s16), buff(), sizeof(s16));
			out(sizeof(s16));
			break;
		case DATA_TYPE_S32:
			tools::SafeMemcpy(&data.s32Value, sizeof(s32), buff(), sizeof(s32));
			out(sizeof(s32));
			break;
		case DATA_TYPE_S64:
			tools::SafeMemcpy(&data.s64Value, sizeof(s64), buff(), sizeof(s64));
			out(sizeof(s64));
			break;
		case DATA_TYPE_DOUBLE:
			tools::SafeMemcpy(&data.dValue, sizeof(double), buff(), sizeof(double));
			out(sizeof(double));
			break;
		case DATA_TYPE_STRING:
			{
				size_t nStrLen = 0;
				tools::SafeMemcpy(&nStrLen, sizeof(size_t), buff(), sizeof(size_t));
				out(sizeof(size_t));
				data.strValue = NEW string(buff(), nStrLen);
				out(nStrLen);
			}
			break;
		}
		--m_nDataNum;
		return *this;
	}

	size_t size() {
		return tlib::TStream<DEFAULT_BUF_LEN, false>::size();
	}

	size_t GetDataNum() {
		return m_nDataNum;
	}
	
	bool IsEmpty() {
		return m_nDataNum == 0;
	}

private:
	size_t m_nDataNum;
};

#endif