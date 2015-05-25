/* 
 * File:   Configmgr.cpp
 * Author: alax
 * 
 * Created on March 2, 2015, 2:16 PM
 */

#include <string>

#include "Configmgr.h"
#include "Tools.h"
#include "tinyxml/tinyxml.h"

Configmgr::Configmgr() {
}

Configmgr::~Configmgr() {
}

IConfigmgr * Configmgr::getInstance() {
    static Configmgr * p = NULL;
    if (NULL == p) {
        p = NEW Configmgr;
        if (!p->Redry()) {
            TASSERT(false, "configmgr module cant be ready");
            delete p;
            p = NULL;
        }
    }

    return p;
}

bool Configmgr::Redry() {
    return LoadCoreConfig() && LoadModuleConfig();
}

bool Configmgr::Initialize() {
    return true;
}

bool Configmgr::Destory() {
    delete this;
    return true;
}

inline const sCoreConfig * Configmgr::GetCoreConfig() {
    return &m_oCoreConfig;
}

inline const sModuleConfig * Configmgr::GetModuleConfig() {
    return &m_oModuleConfig;
}

bool Configmgr::LoadCoreConfig() {
    TiXmlDocument doc;
    string coreConfigPath = string(tools::GetAppPath()) + "/config/core.xml";
    if (!doc.LoadFile(coreConfigPath.c_str())) {
        TASSERT(false, "can't find core file : %s", coreConfigPath.c_str());
        return false;
    }

    const TiXmlElement * pRoot = doc.RootElement();
    TASSERT(pRoot != NULL, "core xml format error");

    const TiXmlElement * p = pRoot->FirstChildElement("loop");
    TASSERT(p != NULL, "core xml format error, can't find child loop");
    m_oCoreConfig.sLoopduration = tools::StringAsInt(p->Attribute("tick"));

    p = pRoot->FirstChildElement("netthread");
    TASSERT(p != NULL, "core xml format error, can't find child netthread");
    m_oCoreConfig.sNetThdCount = tools::StringAsInt(p->Attribute("count"));
    m_oCoreConfig.sNetframetick = tools::StringAsInt(p->Attribute("frametick"));
    m_oCoreConfig.sNetSendSize = tools::StringAsInt(p->Attribute("send_size"));
    m_oCoreConfig.sNetRecvSize = tools::StringAsInt(p->Attribute("recv_size"));

    p = pRoot->FirstChildElement("httpthread");
    TASSERT(p != NULL, "core xml format error, can't find child httpthread");
    m_oCoreConfig.sHttpThdCount = tools::StringAsInt(p->Attribute("count"));
    m_oCoreConfig.sHttpframetick = tools::StringAsInt(p->Attribute("frametick"));

    p = pRoot->FirstChildElement("timer");
    TASSERT(p != NULL, "core xml format error, can't find child timer");
    m_oCoreConfig.sTimerlooptick = tools::StringAsInt(p->Attribute("tick"));

    return true;
}

bool Configmgr::LoadModuleConfig() {
    TiXmlDocument doc;
    string coreConfigPath = string(tools::GetAppPath()) + "/config/module.xml";
    if (!doc.LoadFile(coreConfigPath.c_str())) {
        TASSERT(false, "can't find module file : %s", coreConfigPath.c_str());
        return false;
    }

    const TiXmlElement * pRoot = doc.RootElement();
    TASSERT(pRoot, "module.xml format error");
    const char * pPath = pRoot->Attribute("path");
    TASSERT(pPath, "module.xml format error, can't find module path");
    m_oModuleConfig.strModulePath = pPath;
    
    const TiXmlElement * pModule = pRoot->FirstChildElement("module");
    while (pModule) {
        const char * pName = pModule->Attribute("name");
        TASSERT(pName, "module.xml form error, can't find module's name");
        
        m_oModuleConfig.vctModules.push_back(pName);
        
        pModule = pModule->NextSiblingElement("module");
    }
    
    return true;
}
