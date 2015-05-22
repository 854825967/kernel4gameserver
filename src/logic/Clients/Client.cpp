#include "Client.h"
#include "PublicHeader.h"

tlib::TPool<Client> Client::s_oClientPool;
s32 Client::s_nLinkCount = 0;
