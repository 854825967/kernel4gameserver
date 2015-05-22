#ifndef __CRand_h__
#define __CRand_h__

#include "MultiSys.h"

class CRand {
public:
    virtual ~CRand() {

    }

    void SetSeed(u32 seed) {
        m_lSeed = seed;
    }

    s32 Rand() {
        return ( ((m_lSeed = m_lSeed * 214013L + 2531011L) >> 16) & 0x7fff);
    }

private:
    u64 m_lSeed;
};

#endif //__CRand_h__
