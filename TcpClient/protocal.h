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
