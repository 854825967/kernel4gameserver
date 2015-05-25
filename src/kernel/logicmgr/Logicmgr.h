#ifndef __Logicmgr_h__
#define __Logicmgr_h__

#include "IModule.h"
#include "ILogicmgr.h"
#include "tinyxml/tinyxml.h"

#include <string>
#include <map>
#include <vector>
using namespace std;

class Logicmgr : public ILogicmgr {
public:
    static ILogicmgr * getInstance();
    virtual bool Redry();
    virtual bool Initialize();
    virtual bool Destory();
    virtual IModule * FindModule(const char * pStrModuleName);

private:
    Logicmgr();
    ~Logicmgr();

private:
    vector<IModule *> m_vctModules;
    map<string, IModule *> m_mapModules;
};


#endif //__Logicmgr_h__
