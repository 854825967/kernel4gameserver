#ifndef __CArgs_h__
#define __CArgs_h__


#include "MultiSys.h"
#include "CData.h"
#include "Tools.h"
#include <list>

using namespace std;

#define MAX_BUFF_LEN 2048
class CArgs {
public:
	CArgs()
		: m_dataNum(0)
		, m_dataLength(0)
		, m_Inputpos(0)
		, m_Outputpos(0)
	{
		memset(m_buff, 0, sizeof(m_buff));
	}

	void Init() {
		m_dataNum = 0;
		m_dataLength = 0;
		m_Inputpos = 0;
		m_Outputpos = 0;
		memset(m_buff, 0, sizeof(m_buff));
	}

	CArgs& operator<<(const CData& data) {
		tools::SafeMemcpy(m_buff + m_Inputpos, sizeof(s8), &data.type, sizeof(s8));
		m_Inputpos += sizeof(s8);
		switch (data.type)
		{
		case DATA_TYPE_S8:
			tools::SafeMemcpy(m_buff + m_Inputpos, MAX_BUFF_LEN - m_Inputpos, &data.s8Value, sizeof(s8));
			m_Inputpos += sizeof(s8);
			break;
		case DATA_TYPE_S16:
			tools::SafeMemcpy(m_buff + m_Inputpos, MAX_BUFF_LEN - m_Inputpos, &data.s16Value, sizeof(s16));
			m_Inputpos += sizeof(s16);
			break;
		case DATA_TYPE_S32:
			tools::SafeMemcpy(m_buff + m_Inputpos, MAX_BUFF_LEN - m_Inputpos, &data.s32Value, sizeof(s32));
			m_Inputpos += sizeof(s32);
			break;
		case DATA_TYPE_S64:
			tools::SafeMemcpy(m_buff + m_Inputpos, MAX_BUFF_LEN - m_Inputpos, &data.s64Value, sizeof(s64));
			m_Inputpos += sizeof(s64);
			break;
		case DATA_TYPE_DOUBLE:
			tools::SafeMemcpy(m_buff + m_Inputpos, MAX_BUFF_LEN - m_Inputpos, &data.dValue, sizeof(double));
			m_Inputpos += sizeof(double);
			break;
		case DATA_TYPE_STRING:
			{
				size_t nStrLen = data.strValue->length();
				tools::SafeMemcpy(m_buff + m_Inputpos, MAX_BUFF_LEN - m_Inputpos, &nStrLen, sizeof(size_t));
				m_Inputpos += sizeof(size_t);
				tools::SafeMemcpy(m_buff + m_Inputpos, MAX_BUFF_LEN - m_Inputpos, data.strValue->c_str(), data.strValue->length());
				m_Inputpos += data.strValue->length();
			}
			break;
		default:
			break;
		}
		m_dataLength += data.GetDataSize();
		++m_dataNum;
		return *this;
	}

	CArgs& operator>>(CData& data) {
		data.type = DATA_TYPE_INVALID;
		tools::SafeMemcpy(&data.type, sizeof(s8), m_buff + m_Outputpos, sizeof(s8));
		m_Outputpos += sizeof(s8);
		switch (data.type)
		{
		case DATA_TYPE_S8:
			tools::SafeMemcpy(&data.s8Value, sizeof(s8), m_buff + m_Outputpos, sizeof(s8));
			m_Outputpos += sizeof(s8);
			break;
		case DATA_TYPE_S16:
			tools::SafeMemcpy(&data.s16Value, sizeof(s16), m_buff + m_Outputpos, sizeof(s16));
			m_Outputpos += sizeof(s16);
			break;
		case DATA_TYPE_S32:
			tools::SafeMemcpy(&data.s32Value, sizeof(s32), m_buff + m_Outputpos, sizeof(s32));
			m_Outputpos += sizeof(s32);
			break;
		case DATA_TYPE_S64:
			tools::SafeMemcpy(&data.s64Value, sizeof(s64), m_buff + m_Outputpos, sizeof(s64));
			m_Outputpos += sizeof(s64);
			break;
		case DATA_TYPE_DOUBLE:
			tools::SafeMemcpy(&data.dValue, sizeof(double), m_buff + m_Outputpos, sizeof(double));
			m_Outputpos += sizeof(double);
			break;
		case DATA_TYPE_STRING:
			{
				size_t nStrLen = 0;
				tools::SafeMemcpy(&nStrLen, sizeof(size_t), m_buff + m_Outputpos, sizeof(size_t));
				m_Outputpos += sizeof(size_t);
				data.strValue = NEW string(m_buff + m_Outputpos, nStrLen);
				m_Outputpos += data.strValue->length();
			}
			break;
		default:
			break;
		}
		m_dataLength -= data.GetDataSize();
		--m_dataNum;
		return *this;
	}

	size_t GetDataNum(void) {
		return m_dataNum;
	}
	size_t GetDataLength(void) {
		return m_dataLength;
	}
	bool IsEmpty(void) {
		return m_dataNum == 0;
	}
private:
	size_t m_dataNum;
	size_t m_dataLength;
	char m_buff[MAX_BUFF_LEN];
	size_t m_Inputpos;
	size_t m_Outputpos;
};

#endif