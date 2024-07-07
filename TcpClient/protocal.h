#ifndef PROTOCAL_H
#define PROTOCAL_H


#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef unsigned int uint;

enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN = 0, 
    ENUM_MSG_TYPE_REGISTER_REQUEST, 
    ENUM_MSG_TYPE_REGISTER_RESPOND, 
    ENUM_MSG_TYPE_LOGIN_REQUEST, 
    ENUM_MSG_TYPE_LOGIN_RESPOND, 
    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST, 
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND, 
    ENUM_MSG_TYPE_SEARCH_USR_REQUEST, 
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND, 
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST, 
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND, 
    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,
    ENUM_MSG_TYPE_ADD_FRIEND_REJECT,
    ENUM_MSG_TYPE_MAX = 0x00ffffff,
};

struct PDU{
    uint uiPDULen; // total protocal size
    uint uiMsgType;
    char caData[64];
    uint uiMsgLen;
    int caMsg[];
};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCAL_H
