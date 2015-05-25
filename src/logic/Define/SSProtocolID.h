/* 
 * File:   FrameworkProtocolID.h
 * Author: alax
 *
 * Created on March 11, 2015, 10:38 AM
 */

#ifndef __ProtocolID_h__
#define	__ProtocolID_h__

#ifdef	__cplusplus
extern "C" {
#endif

#define MAKE_ID(tag, id) ((tag<<24) + id)
    enum {
        PROTO_CLIENT = 0,
        PROTO_GS = 1,

        //ADD BEFOR THIS
        PROTO_TYPE_COUNT
    };

    namespace Protocol {
        enum {
            CLIENT_MSG_CHOOSE_GATE_REQ = MAKE_ID(PROTO_CLIENT, 0x01),
            CLIENT_MSG_LOGIN_REQ = MAKE_ID(PROTO_CLIENT, 0x02),
        };
    }
#ifdef	__cplusplus
}
#endif

#endif	/* __ProtocolID_h__ */

