#ifndef PROTOCAL_H
#define PROTOCAL_H


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <QFile>

typedef unsigned int uint;

#define FLUSH_DIR_OK "flush dir ok" 
#define FLUSH_DIR_FAILED "flush dir failed"
#define PATH_NOT_EXIST "path does not exist"
#define CREATE_DIR_OK "create dir ok" 
#define CREATE_DIR_EXIST "created dir already exist"
#define DEL_FRIEND_OK "delete friend ok"
#define DEL_FRIEND_FAILED "delete friend failed"
#define DELETE_FILE_OK "delete file ok"
#define DELETE_FILE_FAILED "delete file failed"
#define RENAME_FILE_OK "rename file ok"
#define RENAME_FILE_FAILED "rename file failed"
#define ENTRY_DIR_OK "entry dir ok"
#define ENTRY_DIR_FAILED "entry dir failed"
#define PRE_DIR_OK "return pre dir ok" 
#define PRE_DIR_FAILED "return pre dir failed"
#define UPLOAD_FILE_OK "upload file ok"
#define UPLOAD_FILE_FAILED "upload file failed"
#define UPLOAD_FILE_START "start upload file data"
#define DOWNLOAD_FILE_OK "download file ok"
#define DOWNLOAD_FILE_FAILED "download file failed"
#define DOWNLOAD_FILE_START "start download file data"

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
    ENUM_MSG_TYPE_FLSUH_FRIEND_REQUEST,
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,
    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND,
    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,
    ENUM_MSG_TYPE_FLUSH_DIR_REQUEST,
    ENUM_MSG_TYPE_FLUSH_DIR_RESPOND,
    ENUM_MSG_TYPE_DELETE_FILE_REQUEST,
    ENUM_MSG_TYPE_DELETE_FILE_RESPOND,
    ENUM_MSG_TYPE_RENAME_FILE_REQUEST,
    ENUM_MSG_TYPE_RENAME_FILE_RESPOND,
    ENUM_MSG_TYPE_ENTRY_DIR_REQUEST,
    ENUM_MSG_TYPE_ENTRY_DIR_RESPOND,
    ENUM_MSG_TYPE_PRE_DIR_REQUEST,
    ENUM_MSG_TYPE_PRE_DIR_RESPOND,
    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,
    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,
    ENUM_MSG_TYPE_MAX = 0x00ffffff,
};

struct PDU{
    uint uiPDULen; // total protocal size
    uint uiMsgType;
    char caData[64];
    uint uiMsgLen;
    int caMsg[];
};

// File info struct
struct FileInfo
{
    char caName[32];       // File Name
    bool bIsDir;           // whether is dir, 1: dir 0: file;
    long long uiSize;      // file size
    char caTime[128];      // last modified time
};

// File struct when doing uploads and downloads
struct TransFile
{
    QFile file; // QFile obj
    qint64 iTotalSize; //total file size
    qint64 iReceivedSize; // received size
    bool bTransform; // whether it's performing transmission
};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCAL_H
