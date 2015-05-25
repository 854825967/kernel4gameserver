#ifndef __PublicHeader_h__
#define __PublicHeader_h__

#include "MultiSys.h"

#pragma pack(push)
#pragma pack(1)
enum {
    TEST_MSG_ID_SET_GROUP = 0,
    TEST_MSG_ID_BROADCAST = 1,
};

struct testmsgheader {
    s32 size;
    s32 msgid;
    s64 ltick;
    void * pThis;
};
#pragma pack(pop)

#endif //__PublicHeader_h__
