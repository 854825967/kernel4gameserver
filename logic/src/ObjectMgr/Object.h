#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "Attrib.h"

class CObject : public CAttrib
{
public:
	bool SetType(const char * pType);
protected:
	string m_strType;;
};


#endif