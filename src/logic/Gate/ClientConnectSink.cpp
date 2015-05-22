/* 
 * File:   ClientConnectSick.cpp
 * Author: alax
 *
 * Created on March 3, 2015, 1:06 PM
 */

#include "ClientConnectSink.h"

SINK_LIST_MAP ClientConnectSink::s_oGroupMap;
tlib::TPool<ClientConnectSink> ClientConnectSink::s_oPoolClient;
s32 ClientConnectSink::s_nLinkCount = 0;
