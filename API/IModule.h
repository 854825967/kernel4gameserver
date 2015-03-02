#ifndef __IModule_h__
#define __IModule_h__

#include "MultiSys.h"
#include "Tools.h"

class IKernel;

#define MODULE_NAME_LENGTH 64

class IModule {
public:
    virtual bool Initialize(IKernel * pKernel) = 0;
    virtual bool Launched(IKernel * pKernel) = 0;
    virtual bool Destroy(IKernel * pKernel) = 0;

public:

    IModule() {
        m_pNextModule = NULL;
        m_pName[MODULE_NAME_LENGTH - 1] = 0;
    }

    bool SetNext(IModule * & pModule) {
        m_pNextModule = pModule;
        return true;
    }

    IModule * GetNext() {
        return m_pNextModule;
    }

    bool SetName(const char * pName) {
        memcpy(m_pName, pName, MODULE_NAME_LENGTH - 1);
        return true;
    }

    const char * GetName() {
        return m_pName;
    }

private:
    IModule * m_pNextModule;
    char m_pName[MODULE_NAME_LENGTH];
};

typedef IModule * (*GetModuleFun)(void);

#define NAME_OF_GET_LOGIC_FUN "GetLogicModule"
#define GET_LOGIC_FUN GetLogicModule

#ifndef KERNEL_PROCESS
#define CREATE_MODULE(name) \
    class factroy##name    \
    {    \
    public:    \
        factroy##name(IModule * & pModule)    \
        {    \
            IModule * pModule##name = NEW name;    \
            pModule##name->SetName(#name);    \
            pModule##name->SetNext(pModule); \
            pModule = pModule##name;    \
        }    \
    };    \
    factroy##name factroy##name(plogicModule);


#define GET_DLL_ENTRANCE \
static IModule * plogicModule = NULL; \
extern "C" IModule * GET_LOGIC_FUN() {    \
    srand(tools::GetTimeMillisecond()); \
    return plogicModule; \
}

#endif    //KERNEL_PROCESS

#endif  //__IModule_h__
