/*
 * File:   Logicmgr.cpp
 * Author: traveler
 *
 * Created on December 18, 2012, 6:13 PM
 */

#include "Logicmgr.h"
#include <dlfcn.h>
#include "IKernel.h"
#include "configmgr/Configmgr.h"
#include "Kernel.h"

Logicmgr::Logicmgr() {

}

ILogicmgr * Logicmgr::getInstance() {
    static ILogicmgr * p = NULL;
    if (NULL == p) {
        p = NEW Logicmgr;
        if (!p->Redry()) {
            TASSERT(false, "Logicmgr cant ready");
            delete p;
            p = NULL;
        }
    }

    return p;
}

bool Logicmgr::Redry() {
    return true;
}

bool Logicmgr::Initialize() {
    const sModuleConfig * pConfig = Configmgr::getInstance()->GetModuleConfig();
    TASSERT(pConfig, "can't load module config");

    vector<string>::const_iterator itor = pConfig->vctModules.begin();
    vector<string>::const_iterator iend = pConfig->vctModules.end();
    while (itor != iend) {
        char path[512] = {0};
        SafeSprintf(path, sizeof (path), "%s/%s/lib%s.so",
                tools::GetAppPath(), pConfig->strModulePath.c_str(), (*itor).c_str());

        ECHO("module path : %s", path);
        void * handle = dlopen(path, RTLD_LAZY);
        TASSERT(handle, "open %s error", path);

        GetModuleFun fun = (GetModuleFun) dlsym(handle, "GetLogicModule");
        TASSERT(fun, "get function:GetLogicModule error");

        IModule * plogic = fun();
        TASSERT(plogic, "can't get module from lig%s.so", (*itor).c_str());

        while (plogic) {
            const char * pName = plogic->GetName();
            map<string, IModule *>::iterator mitor = m_mapModules.find(pName);
            if (mitor != m_mapModules.end()) {
                TASSERT(false, "LogicModule Name %s is exists", pName);
                return false;
            }

            m_vctModules.push_back(plogic);
            m_mapModules.insert(make_pair(pName, plogic));
            plogic = plogic->GetNext();
        }

        {
            vector<IModule *>::iterator vitor = m_vctModules.begin();
            vector<IModule *>::iterator viend = m_vctModules.end();
            while (vitor != viend) {
                (*vitor)->Initialize(Kernel::getInstance());
                
                vitor ++;
            }
            
            vitor = m_vctModules.begin();
            while(vitor != viend) {
                (*vitor)->Launched(Kernel::getInstance());
                vitor++;
            }
        }


        itor++;
    }

    return true;
}

bool Logicmgr::Destory() {
    delete this;
    return true;
}

Logicmgr::~Logicmgr() {
    //    map<string, IModule *>::iterator itor = m_mapModules.begin();
    //    map<string, IModule *>::iterator iend = m_mapModules.end();
    //    while (itor != iend) {
    //        TASSERT(itor->second != NULL, "where is module?");
    //        if (NULL != itor->second) {
    //            itor->second->Destroy(m_pKernel);
    //            delete itor->second;
    //            itor->second = NULL;
    //        }
    //        itor++;
    //    }
    //    m_mapModules.clear();
    //    
    //    vector<void *>::iterator ivbegin = m_vctModules.begin();
    //    vector<void *>::iterator ivend = m_vctModules.end();
    //    while(ivbegin != ivend) {
    //        if(*ivbegin != NULL) {
    //            dlclose(*ivbegin);
    //        }
    //        ivbegin ++;
    //    }
    //    m_vctModules.clear();
}

IModule * Logicmgr::FindModule(const char * pStrModuleName) {
    map<string, IModule *>::iterator itor = m_mapModules.find(pStrModuleName);
    if (itor == m_mapModules.end() || NULL == itor->second) {
        TASSERT(false, (string("There is no LogciModule named %s") + pStrModuleName).c_str());
        return NULL;
    }

    return itor->second;
}
