#include "Logger.h"
#include "configmgr/Configmgr.h"

ILogger * Logger::getInstance() {
    static Logger * p = NULL;
    if (NULL == p) {
        p = NEW Logger;
        if (!p->Redry()) {
            TASSERT(false, "Logger cant ready");
            delete p;
            p = NULL;
        }
    }

    return p;
}

bool Logger::Redry() {
    m_oFileInfo.file = NULL;
    m_oFileInfo.m_lFileSize = 0;
    m_oFileInfo.m_lOpenTick = 0;
    m_nStatus = THREAD_STOPED;

    m_lMaxTick = 60 * 60 * 1000;
    m_lMaxSize = 1024 * 1024 * 256;
    return true;
}

bool Logger::Initialize() {
    Start();
    return true;
}

bool Logger::Destory() {
    delete this;
    return true;
}

void Logger::Run() {
    m_nStatus = THREAD_WORKING;
    while (true) {
        LOGStream * pLog = NULL;
        if (m_oQueueStream.Read(pLog)) {
            s64 lTick = tools::GetTimeMillisecond();
            if (NULL == m_oFileInfo.file) {
                char szName[256] = {0};
                SafeSprintf(szName, sizeof(szName), "%s/%s%s.log", tools::GetAppPath(), "logic_log_", tools::GetCurrentTimeString("%4d-%02d-%02d-%02d-%02d-%02d").c_str());
                if (!m_oFileInfo.FILEOpen(szName)) {
                    ECHO_ERROR("create log file error, logger stop working");
                }
            }
             
            if (lTick - m_oFileInfo.m_lOpenTick > m_lMaxTick || m_oFileInfo.m_lFileSize > m_lMaxSize) {
                char szName[256] = {0};
                SafeSprintf(szName, sizeof(szName), "%s/%s%s.log", tools::GetAppPath(), "logic_log_", tools::GetCurrentTimeString("%4d-%02d-%02d-%02d-%02d-%02d").c_str());
                if (!m_oFileInfo.FILEClose() || !m_oFileInfo.FILEOpen(szName)) {
                    ECHO_ERROR("create log file error, logger stop working");
                } else {
                    m_oFileInfo.FILEWrite(pLog->buff(), pLog->size());
                }
            } else {
                m_oFileInfo.FILEWrite(pLog->buff(), pLog->size());
            }


            m_oPoolStream.Recover(pLog);
        } else if (m_oQueueStream.IsEmpty()) {
            if (THREAD_STOPPING == m_nStatus) {
                m_nStatus = THREAD_STOPED;
                return;
            } else {
                CSLEEP(1);
            }
        }
    }
}

bool Logger::Terminate() {
    if (THREAD_WORKING == m_nStatus) {
        m_nStatus = THREAD_STOPPING;
        while (m_nStatus != THREAD_STOPED) {
            CSLEEP(1);
        }
        return true;
    }

    return false;
}

void Logger::Log(const char * log) {
    TASSERT(log, "log a long point");

    if (NULL == log) {
        return;
    }

    LOGStream * pStream = m_oPoolStream.Create();
    pStream->in(log, strlen(log));
    m_oQueueStream.Add(pStream);
}
