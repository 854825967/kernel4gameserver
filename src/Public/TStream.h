#ifndef __TStream_h__
#define __TStream_h__

#include "MultiSys.h"
#include "CLock.h"
#include "Tools.h"
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
                    delete[] m_pbuff;
                    m_pbuff = NULL;
                }
                m_pbuff = NEW char[buff_length];
                m_max = buff_length;
            }
            memset(m_pbuff, 0, buff_length);

            if (b) {
                m_pRlock->Clear();
                m_pWlock->Clear();
            }
        }

        inline const char * buff() const {
            return &m_pbuff[m_read];
        }

        void out(const s32 size) {
            if (size > m_write - m_read) {
                TASSERT(false, "tstream overflow");
                m_read = m_write;
            } else {
                m_read += size;
            }
        }

        inline s32 size() const {
            s32 nSize = 0;
            nSize = m_write - m_read;
            return nSize;
        }

        void in(const void * pbuff, s32 size) {
            s32 cursize = 0;
            if (size > m_max - m_write) {
                cursize = m_write - m_read;
                if (m_max - cursize <= size) {
                    s32 nNewSize = m_max + buff_length;
                    while (nNewSize - cursize <= size) {
                        nNewSize += buff_length;
                    }
                    malloc_new_size(nNewSize);
                } else {
                    STREAM_OPT_LOCK(b, m_pRlock);
                    cursize = m_write - m_read;
                    tools::SafeMemcpy(m_pbuff, m_max,&m_pbuff[m_read], cursize);
                    m_write = cursize;
                    m_read = 0;
                    STREAM_OPT_FREELOCK(b, m_pRlock);
                }
            }

            tools::SafeMemcpy(&m_pbuff[m_write], m_max-m_write, pbuff, size);
            m_write += size;
        }

        inline void LockRead() {
            STREAM_OPT_LOCK(b, m_pRlock);
        }

        inline void FreeRead() {
            STREAM_OPT_FREELOCK(b, m_pRlock);
        }

        inline void LockWrite() {
            STREAM_OPT_LOCK(b, m_pWlock);
        }

        inline void FreeWrite() {
            STREAM_OPT_FREELOCK(b, m_pWlock);
        }

    private:

        void malloc_new_size(s32 newSize) {
            STREAM_OPT_LOCK(b, m_pRlock);
            s32 cursize = m_write - m_read;
            m_max = newSize;
            char * ptemp = m_pbuff;
            m_pbuff = NEW char[m_max];
            tools::SafeMemcpy(m_pbuff, m_max, &ptemp[m_read], cursize);
            delete[] ptemp;
            m_write = cursize;
            m_read = 0;
            STREAM_OPT_FREELOCK(b, m_pRlock);
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

