#ifndef __TStream_h__
#define __TStream_h__

#include "MultiSys.h"
#include "CLock.h"
#include <string.h>

#define STREAM_OPT_LOCK(b, lock) \
    if (b) { \
        lock->Lock(); \
    }

#define STREAM_OPT_FREELOCK(b, lock) \
    if (b) { \
        lock->UnLock(); \
    }


namespace tlib {

    template<s32 buff_length, bool b = false >
    class TStream {
    public:

        TStream() {
            m_pbuff = NEW char[buff_length];
            memset(m_pbuff, 0, buff_length);
            m_read = 0;
            m_write = 0;
            m_max = buff_length;

            if (b) {
                m_pRlock = NEW CLockUnit;
                m_pWlock = NEW CLockUnit;
            }
        }

        ~TStream() {
            if (m_pbuff != NULL) {
                delete m_pbuff;
                m_pbuff = NULL;
            }
        }

        void clear() {
            m_read = 0;
            m_write = 0;
            if (m_max > buff_length) {
                if (m_pbuff != NULL) {
                    delete m_pbuff;
                    m_pbuff = NULL;
                }
                m_pbuff = NEW char[buff_length];
                m_max = buff_length;
            }
            memset(m_pbuff, 0, buff_length);
        }

        const char * buff() const {
            return &m_pbuff[m_read];
        };

        void out(const s32 size) {
            STREAM_OPT_LOCK(b, m_pRlock);
            if (size > m_write - m_read) {
                TASSERT(false, "tstream overflow");
                m_read = m_write;
            } else {
                m_read += size;
            }
            STREAM_OPT_FREELOCK(b, m_pRlock);
        }

        s32 size() const {
            return m_write - m_read;
        }

        void in(const void * pbuff, s32 size) {
            STREAM_OPT_LOCK(b, m_pWlock);
            if (size > m_max - m_write) {
                s32 cursize = m_write - m_read;
                if (m_max - cursize <= size) {
                    while (m_max - cursize < size) {
                        malloc_double();
                    }
                } else {
                    STREAM_OPT_LOCK(b, m_pRlock);
                    memcpy(m_pbuff, &m_pbuff[m_read], cursize);
                    m_write = cursize;
                    m_read = 0;
                    STREAM_OPT_FREELOCK(b, m_pRlock);
                }
            }

            memcpy(&m_pbuff[m_write], pbuff, size);
            m_write += size;
            STREAM_OPT_FREELOCK(b, m_pWlock);
        }

        void LockRead() {
            STREAM_OPT_LOCK(b, m_pRlock);
        }

        void FreeRead() {
            STREAM_OPT_FREELOCK(b, m_pRlock);
        }

        void LockWrite() {
            STREAM_OPT_LOCK(b, m_pWlock);
        }

        void FreeWrite() {
            STREAM_OPT_FREELOCK(b, m_pWlock);
        }

    private:

        void malloc_double() {
            STREAM_OPT_LOCK(b, m_pRlock);
            s32 cursize = m_write - m_read;
            m_max *= 2;
            char * ptemp = m_pbuff;
            m_pbuff = NEW char[m_max];
            memcpy(m_pbuff, &ptemp[m_read], cursize);
            delete ptemp;
            m_write = cursize;
            m_read = 0;
            STREAM_OPT_FREELOCK(b, m_pRlock);
        }

        void half_free() {
            if (m_max > buff_length) {
                s32 cursize = m_write - m_read;
                if (m_max > 4 * cursize) {
                    STREAM_OPT_LOCK(b, m_pWlock);
                    STREAM_OPT_LOCK(b, m_pRlock);
                    m_max /= 2;
                    char * ptemp = m_pbuff;
                    m_pbuff = NEW char[m_max];
                    memcpy(m_pbuff, &ptemp[m_read], cursize);
                    delete ptemp;
                    m_write = cursize;
                    m_read = 0;
                    STREAM_OPT_FREELOCK(b, m_pRlock);
                    STREAM_OPT_FREELOCK(b, m_pWlock);
                }
            }
        }

    private:
        CLockUnit * m_pRlock;
        CLockUnit * m_pWlock;
        char * m_pbuff;
        s32 m_read;
        s32 m_write;
        s32 m_max;
    };
}

#endif //defined __TStream_h__

