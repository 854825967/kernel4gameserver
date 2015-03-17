#include "Object.h"


bool CObject::SetType(const char * pType) {
	m_strType = pType;
	return Init(pType);
}

