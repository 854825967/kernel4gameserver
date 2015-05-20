#include "MultiSys.h"
#include "Tools.h"
#include <assert.h>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include "CLock.h"
using namespace tlib;
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

void _AssertionFail(const char * strFile, int nLine, const char * pFunName, const char * debug) {
    fflush(stdout);
    fprintf(stderr,
        "\nAsssertion failed : %s\n=======assert string=======\nfile:%s\nline:%d\nfunction:%s\ndebug:%s\n===========================\n",
        tools::GetCurrentTimeString().c_str(), strFile, nLine, pFunName, debug);
    fflush(stderr);
    assert(false);
}

#ifdef __cplusplus
}

typedef map<void *, string> MEM_MAP;
typedef map<ThreadID, MEM_MAP> THREAD_MEM_MAP;

static THREAD_MEM_MAP * s_pThreadMemMap = NULL;
static CLockUnit s_lock;

void TraceMemory() {
    if (NULL == s_pThreadMemMap) {
        s_pThreadMemMap = new THREAD_MEM_MAP;
    }
    string path = string(tools::GetAppPath()) + "\\" + string("log_") + tools::Int64AsString(tools::GetTimeMillisecond()) + ".log";
    ofstream ofs(path, ios::out);

    THREAD_MEM_MAP::iterator itor = s_pThreadMemMap->begin();
    THREAD_MEM_MAP::iterator iend = s_pThreadMemMap->end();
    while (itor != iend) {
        {
            MEM_MAP::iterator mitor = itor->second.begin();
            MEM_MAP::iterator miend = itor->second.end();
            while (mitor != miend) {

                char szBuff[512] = {0};
                SafeSprintf(szBuff, sizeof(szBuff), "%lx, %s", mitor->first, mitor->second.c_str());
                ofs << szBuff << endl;

                mitor ++;
            }
        }

        itor ++;
    }

    ofs.close();

    return;
}

//为了找到内存泄露的元凶，我要记录下每一处new所在的文件名和所在行。于是再次重载了operator new：
void * operator new(size_t size, const char* file, const size_t line) {
    if (0 == size) {
        TASSERT(false, "wtf new");
        return NULL;
    }

    if (NULL == s_pThreadMemMap) {
        s_pThreadMemMap = new THREAD_MEM_MAP;
    }

    ThreadID threadID = tools::GetCurrentThreadID();

    THREAD_MEM_MAP::iterator itor = s_pThreadMemMap->find(threadID);
    if (itor == s_pThreadMemMap->end()) {
        s_pThreadMemMap->insert(make_pair(threadID, MEM_MAP()));
        itor = s_pThreadMemMap->find(threadID);
    }

    void *p = malloc(size);
    string path = string(file) + " || " + tools::IntAsString(line).c_str() + " || " + tools::IntAsString(line).c_str();
    itor->second.insert(make_pair(p, path));

    return p;

}

void * operator new[](size_t size, const char* file, const size_t line) {
    return operator new(size, file, line);
}
// 
// //为了避免编译时出现warning C4291(没有与operator new(unsigned int,const char *,const unsigned int) 匹配的delete)，又重载了
// void operator delete (void * pointer) {
//     if (NULL == pointer) {
//         TASSERT(false, "wtf delete");
//         return;
//     }
// 
//     CLock lock(&s_lock);    
// 
//     THREAD_MEM_MAP::iterator itor = s_ThreadMemMap.begin();
//     THREAD_MEM_MAP::iterator iend = s_ThreadMemMap.end();
//     bool res = false;
//     while (itor != iend) {
//         {
//             MEM_MAP::iterator mitor = itor->second.find(pointer);
//             if (mitor != itor->second.end()) {
//                 itor->second.erase(mitor);
//                 res = true;
//                 break;
//             }
//         }
// 
//         itor ++;
//     }
// 
//     return;
// }
// 
// void operator delete[](void * pointer) {
//     return operator delete(pointer);
// }


#endif

