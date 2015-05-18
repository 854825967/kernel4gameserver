#ifndef __Logger_h__
#define __Logger_h__
#include "ILogger.h"
#include "CThread.h"
#include "TStream.h"
#include "TQueue.h"
#include "TPool.h"
using namespace tlib;

struct FileInfo {
    FILE * file;
    s64 m_lOpenTick;
    s64 m_lFileSize;

    bool FILEOpen(const char * name) {
        if (file != NULL) {
            TASSERT(false, "create file error %d", ::GetLastError());
            return false;
        }

        file = fopen(name, "a+");
        if (NULL == file) {
            return false;
        }
        m_lFileSize = 0;
        m_lOpenTick = tools::GetTimeMillisecond();
        return true;
    }

    bool FILEClose() {
        if (NULL == file) {
            return false;
        }

        if (0 == fclose(file)) {
            file = NULL;
            m_lOpenTick = 0;
            m_lFileSize = 0;
            return true;
        } else {
            TASSERT(false, "close file error %d", ::GetLastError());
        }

        return false;
    }

    bool FILEWrite(const char * context, const s32 size) {
        s32 nCount = fwrite(context, 1, size, file);
        m_lFileSize += nCount;
        if (nCount != size) {
            TASSERT(false, "write file error %d", ::GetLastError());
            return false;
        }
        fflush(file);
        return true;
    }
};

typedef TStream<1024, false> LOGStream;

class Logger : public ILogger, CThread {
public:

    static ILogger * getInstance();

    virtual bool Redry();
    virtual bool Initialize();
    virtual bool Destory();

    virtual void Run();
    virtual bool Terminate();

    virtual void Log(const char * log);
private:
    s64 m_lMaxTick;
    s64 m_lMaxSize;
    FileInfo m_oFileInfo;
    
    TPool<LOGStream, true, 256, 64> m_oPoolStream;
    TQueue<LOGStream *, false, 4096> m_oQueueStream;
};

#endif //__Logger_h__
