/* 
 * File:   ClientConnectSick.cpp
 * Author: alax
 * 
 * Created on March 3, 2015, 1:06 PM
 */

#include "ClientConnectSick.h"

ClientConnectSick::ClientConnectSick() {

}

void ClientConnectSick::Error(const s8 opt, const s32 code) {
    
}

s32 ClientConnectSick::Recv(void * context, const s32 size) {
    ECHO("%s", (const char *) context);
    return size;
}

void ClientConnectSick::Disconnect() {
    ECHO("connection breaked");
}

void ClientConnectSick::Connected() {
    ECHO("connection connected");
}
