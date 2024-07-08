#include "mytcpsocket.h"
#include "mytcpserver.h"
#include <QDebug>
#include <QDir>
#include <QDateTime>

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this, SIGNAL(readyRead()), this, SLOT(recvMsg()));

    connect(this, SIGNAL(disconnected()), this, SLOT(clientOffline()));
}

void MyTcpSocket::recvMsg(){
    qDebug() << this->bytesAvailable();
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU * pdu = mkPDU(uiMsgLen);
    this->read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));
    //qDebug() << pdu->uiMsgType << " --> " << (char*) pdu -> caMsg;

    switch (pdu->uiMsgType){
        case ENUM_MSG_TYPE_REGISTER_REQUEST:
        {
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);
            strncpy(caPwd, pdu->caData + 32, 32);
            qDebug() << caName << caPwd << pdu->uiMsgType;
            bool ret = OpeDB::getInstance().handleRegist(caName, caPwd);
            PDU * respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_REGISTER_RESPOND;
            if (ret){
                strcpy(respdu->caData, "Register is OK ! !");

                QDir dir;
                ret = dir.mkdir(QString("%1/%2").arg(MyTcpServer::getInstance().getStrRootPath()).arg(caName));
                qDebug() << "Creating New Folder for new user " << ret;
            }
            else{strcpy(respdu->caData, "Register ff");}

            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;

        }

        case ENUM_MSG_TYPE_LOGIN_REQUEST:
        {
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName, pdu->caData, 32);
            strncpy(caPwd, pdu->caData + 32, 32);
            //qDebug() << caName << caPwd << pdu->uiMsgType;
            bool ret = OpeDB::getInstance().handleLogin(caName, caPwd);
            PDU *respdu = NULL;
            
            if (ret){
                QString strUserRootPath = QString("%1/%2")
                    .arg(MyTcpServer::getInstance().getStrRootPath()).arg(caName);
                qDebug() << "Logged User Root Path:" << strUserRootPath;
                respdu = mkPDU(strUserRootPath.size() + 1);
                strncpy((char*)respdu -> caMsg, strUserRootPath.toStdString().c_str(), strUserRootPath.size() + 1);
                strcpy(respdu->caData, "Login is OK ! !");
                m_strName = caName;
            }
            else{
                respdu = mkPDU(0);
                strcpy(respdu->caData, "Login ff");
            }
            respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }


        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
        {
            QStringList ret = OpeDB::getInstance().handleAllOnline();
            uint uiMsgLen = ret.size() *32;
            PDU *respdu = mkPDU(uiMsgLen);
            respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
            for (int i = 0; i < ret.size(); i++){
                memcpy((char*)respdu->caMsg + i *32, 
                ret.at(i).toStdString().c_str(), 
                ret.at(i).size());
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }

        case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:
        {
            int ret = OpeDB::getInstance().handleSearchUsr(pdu->caData);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;

            if (ret == -1){
                strcpy(respdu->caData, "Not Find");
            }
            else if (ret == 1){
                strcpy(respdu->caData, "Find Online");
            }
            else{
                strcpy(respdu->caData, "Find OffLine");
            }
            
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }


        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            char caPerName[32] = {'\0'};
            char caName[32] = {'\0'};
            strncpy(caPerName, pdu->caData, 32);
            strncpy(caName, pdu->caData + 32, 32);
            int ret = OpeDB::getInstance().handleAddFriend(caPerName, caName);

            PDU* respdu = NULL;
            if (-1 == ret){
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, "Type -1 Error");
                write((char*)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;

            }
            else if (0 == ret){
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, "Type 0 Error");
                write((char*)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            else if (1 == ret){
                MyTcpServer::getInstance().resend(caPerName, pdu);
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, "Valid Request, exists & online");
                write((char*)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            else if (2 == ret){
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, "Type 2 Error");
                write((char*)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            else if (3 == ret){
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strcpy(respdu->caData, "Type 3 Error");
                write((char*)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }
            break;
        }

        case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:
        {
            char addedName[32] = {'\0'};
            char sourceName[32] = {'\0'};
            strncpy(addedName, pdu -> caData, 32);
            strncpy(sourceName, pdu -> caData + 32, 32);
            OpeDB::getInstance().handleAddFriendAgree(addedName, sourceName);
            MyTcpServer::getInstance().resend(sourceName, pdu);
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REJECT: 
        {
            char sourceName[32] = {'\0'};
            strncpy(sourceName, pdu -> caData + 32, 32);
            MyTcpServer::getInstance().resend(sourceName, pdu);
            break;
        }
        case ENUM_MSG_TYPE_FLSUH_FRIEND_REQUEST: 
        {
            char caName[32] = {'\0'};
            strncpy(caName, pdu -> caData, 32);

            QStringList strList = OpeDB::getInstance().handleFlushFriendRequest(caName);
            uint uiMsgLen = strList.size() / 2 * 36;

            PDU* respdu = mkPDU(uiMsgLen);
            respdu -> uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
            for(int i = 0; i * 2 < strList.size(); ++ i){
                strncpy((char*)(respdu -> caMsg) + 36 * i, strList.at(i * 2).toStdString().c_str(), 32);
                strncpy((char*)(respdu -> caMsg) + 36 * i + 32, strList.at(i * 2 + 1).toStdString().c_str(), 4);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            break;
        }
        
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST: 
        {
            char deletedName[32] = {'\0'};
            char sourceName[32] = {'\0'};
            strncpy(deletedName, pdu -> caData, 32);
            strncpy(sourceName, pdu -> caData + 32, 32);
            bool ret = OpeDB::getInstance().handleDeleteFriend(deletedName, sourceName);

            PDU *respdu = mkPDU(0);
            respdu -> uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
            if(ret){
                strncpy(respdu -> caData, DEL_FRIEND_OK, 32);
            }
            else{
                strncpy(respdu -> caData, DEL_FRIEND_FAILED, 32);
            }
            MyTcpServer::getInstance().resend(deletedName, pdu);
            free(respdu);
            respdu = NULL;
            break;
        }

        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:
        {

            char caDirName[32];
            char caCurPath[pdu -> uiMsgLen];
            strncpy(caDirName, pdu -> caData, 32);
            strncpy(caCurPath, (char*)pdu -> caMsg, pdu -> uiMsgLen);

            QString strDir = QString("%1/%2").arg(caCurPath).arg(caDirName);
            QDir dir;
            PDU *respdu = mkPDU(0);
            respdu -> uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;

            qDebug() << "Handling Creat Dir Req:" << strDir;
            if(dir.exists(caCurPath)){
                if(dir.exists(strDir)){
                    strncpy(respdu -> caData, CREATE_DIR_EXIST, 32);
                }
                else{
                    dir.mkdir(strDir);
                    strncpy(respdu -> caData, CREATE_DIR_OK, 32);
                }
            }
            else{
                qDebug() << "Current path is not Valid, check base path!" << strDir;
                strncpy(respdu -> caData, PATH_NOT_EXIST, 32);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }



        case ENUM_MSG_TYPE_FLUSH_DIR_REQUEST: 
        {
            char caCurDir[pdu -> uiMsgLen];
            memcpy(caCurDir, (char*)pdu -> caMsg, pdu -> uiMsgLen);
            qDebug() << "Socket Handling Retriving Dir Info at: " << caCurDir;
            QDir dir;
            PDU* resPdu = NULL;

            if(!dir.exists(caCurDir)){
                resPdu = mkPDU(0);
                strncpy(resPdu -> caData, PATH_NOT_EXIST, 32);
            }
            else {
                dir.setPath(caCurDir);
                QFileInfoList fileInfoList = dir.entryInfoList(); 
                int iFileNum = fileInfoList.size();

                resPdu = mkPDU(sizeof(FileInfo) * iFileNum);
                FileInfo *pFileInfo = NULL;
                strncpy(resPdu -> caData, FLUSH_DIR_OK, 32);

                for(int i = 0; i < iFileNum; ++ i){
                    pFileInfo = (FileInfo*)(resPdu -> caMsg) + i; // Every shift is (FileInfo*) amount
                    memcpy(pFileInfo -> caName, fileInfoList[i].fileName().toStdString().c_str(), fileInfoList[i].fileName().size());
                    pFileInfo -> bIsDir = fileInfoList[i].isDir();
                    pFileInfo -> uiSize = fileInfoList[i].size();
                    QDateTime dtLastTime = fileInfoList[i].lastModified();
                    QString strLastTime = dtLastTime.toString("yyyy/MM/dd hh:mm");
                    memcpy(pFileInfo -> caTime, strLastTime.toStdString().c_str(), strLastTime.size());
                    qDebug() << "Current File: " << pFileInfo -> caName << " " << pFileInfo -> bIsDir << " " << pFileInfo -> uiSize << " " << pFileInfo -> caTime;
                }
            }
            resPdu -> uiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_RESPOND;

            write((char*)resPdu, resPdu->uiPDULen);
            free(resPdu);
            resPdu = NULL;

            break;
        }


        case ENUM_MSG_TYPE_DELETE_FILE_REQUEST:
        {
            PDU* resPdu = mkPDU(0);
            char strDelPath[pdu -> uiMsgLen];
            memcpy(strDelPath, (char*)pdu -> caMsg, pdu -> uiMsgLen);
            qDebug() << "Socke Del OPs at: " << strDelPath;
            QDir dir;

            resPdu -> uiMsgType = ENUM_MSG_TYPE_DELETE_FILE_RESPOND;
            if(!dir.exists(strDelPath)){
                strncpy(resPdu -> caData, PATH_NOT_EXIST, 32);
            }
            else{
                bool ret = false;
                QFileInfo fileInfo(strDelPath);
                if(fileInfo.isDir()){
                    dir.setPath(strDelPath);
                    ret = dir.removeRecursively();
                }
                else if(fileInfo.isFile()){
                    ret = dir.remove(strDelPath);
                }
                if(ret){
                    strncpy(resPdu -> caData, DELETE_FILE_OK, 32);
                }
                else{
                    strncpy(resPdu -> caData, DELETE_FILE_FAILED, 32);
                }
            }
            qDebug() << resPdu -> caData;
            write((char*)resPdu, resPdu->uiPDULen);
            free(resPdu);
            resPdu = NULL;
            break;
        }

        case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:
        {
            PDU* resPdu = mkPDU(0);
            char caCurPath[pdu -> uiMsgLen];
            char caOldName[32];
            char caNewName[32];
            memcpy(caCurPath, (char*)pdu -> caMsg, pdu -> uiMsgLen);
            strncpy(caOldName, pdu -> caData, 32);
            strncpy(caNewName, pdu -> caData + 32, 32);
            qDebug() << "Server Socket Dir Reanme Ops" << caCurPath << " " << caOldName << " -> " << caNewName;
            QDir dir;

            resPdu -> uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
            dir.setPath(caCurPath);
            if(dir.rename(caOldName, caNewName)){
                strncpy(resPdu -> caData, RENAME_FILE_OK, 32);
            }
            else{
                strncpy(resPdu -> caData, RENAME_FILE_FAILED, 32);
            }
            //qDebug() << resPdu -> caData;
            write((char*)resPdu, resPdu->uiPDULen);
            free(resPdu);
            resPdu = NULL;
            break;
        }

        case ENUM_MSG_TYPE_ENTRY_DIR_REQUEST:
        {
            char strEntryPath[pdu -> uiMsgLen]; // Enter required dir
            memcpy(strEntryPath, (char*)pdu -> caMsg, pdu -> uiMsgLen);
            qDebug() << "Server Socket: Entering the Path " << strEntryPath;
            PDU* resPdu = NULL;
            QDir dir(strEntryPath);

            if(!dir.exists()) { // Dir Not exist
                resPdu = mkPDU(0);
                strncpy(resPdu -> caData, PATH_NOT_EXIST, 32);
                resPdu -> uiMsgType = ENUM_MSG_TYPE_ENTRY_DIR_RESPOND;
            }
            else{
                QFileInfo fileInfo(strEntryPath);
                if(!fileInfo.isDir()) { // Not Dir
                    resPdu = mkPDU(0);
                    strncpy(resPdu -> caData, ENTRY_DIR_FAILED, 32);
                    resPdu -> uiMsgType = ENUM_MSG_TYPE_ENTRY_DIR_RESPOND;
                }
                else{ // Exact same as refresh current dir
                    
                    char caCurDir[pdu -> uiMsgLen];
                    memcpy(caCurDir, (char*)pdu -> caMsg, pdu -> uiMsgLen);
                    QDir dir;
                    dir.setPath(caCurDir);
                    QFileInfoList fileInfoList = dir.entryInfoList(); 
                    int iFileNum = fileInfoList.size();
                    resPdu = mkPDU(sizeof(FileInfo) * iFileNum);
                    FileInfo *pFileInfo = NULL;
                    for(int i = 0; i < iFileNum; ++ i){
                        pFileInfo = (FileInfo*)(resPdu -> caMsg) + i;
                        memcpy(pFileInfo -> caName, fileInfoList[i].fileName().toStdString().c_str(), fileInfoList[i].fileName().size());
                        pFileInfo -> bIsDir = fileInfoList[i].isDir();
                        pFileInfo -> uiSize = fileInfoList[i].size();
                        QDateTime dtLastTime = fileInfoList[i].lastModified();
                        QString strLastTime = dtLastTime.toString("yyyy/MM/dd hh:mm");
                        memcpy(pFileInfo -> caTime, strLastTime.toStdString().c_str(), strLastTime.size());
                    }
                    strncpy(resPdu -> caData, ENTRY_DIR_OK, 32);
                    resPdu -> uiMsgType = ENUM_MSG_TYPE_ENTRY_DIR_RESPOND;

                }
            }

            write((char*)resPdu, resPdu->uiPDULen);
            free(resPdu);
            resPdu = NULL;
            break;
        }


        case ENUM_MSG_TYPE_PRE_DIR_REQUEST:
        {
            char strPrePath[pdu -> uiMsgLen];
            memcpy(strPrePath, (char*)pdu -> caMsg, pdu -> uiMsgLen);
            qDebug() << "Prev Dir: " << strPrePath;
            PDU* resPdu = NULL;
            QDir dir(strPrePath);

            if(!dir.exists()) {
                resPdu = mkPDU(0);
                strncpy(resPdu -> caData, PATH_NOT_EXIST, 32);
            }
            else { // Same as Flush Dir
                char caCurDir[pdu -> uiMsgLen];
                memcpy(caCurDir, (char*)pdu -> caMsg, pdu -> uiMsgLen);
                QDir dir;
                dir.setPath(caCurDir);
                QFileInfoList fileInfoList = dir.entryInfoList(); 
                int iFileNum = fileInfoList.size();
                resPdu = mkPDU(sizeof(FileInfo) * iFileNum);
                FileInfo *pFileInfo = NULL;
                strncpy(resPdu -> caData, FLUSH_DIR_OK, 32);
                for(int i = 0; i < iFileNum; ++ i){
                    pFileInfo = (FileInfo*)(resPdu -> caMsg) + i;
                    memcpy(pFileInfo -> caName, fileInfoList[i].fileName().toStdString().c_str(), fileInfoList[i].fileName().size());
                    pFileInfo -> bIsDir = fileInfoList[i].isDir();
                    pFileInfo -> uiSize = fileInfoList[i].size();
                    QDateTime dtLastTime = fileInfoList[i].lastModified();
                    QString strLastTime = dtLastTime.toString("yyyy/MM/dd hh:mm");
                    memcpy(pFileInfo -> caTime, strLastTime.toStdString().c_str(), strLastTime.size());
                }
                resPdu -> uiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_RESPOND; // use FLUSH TYPE
            }
            resPdu -> uiMsgType = ENUM_MSG_TYPE_PRE_DIR_RESPOND;
            qDebug() << "1 resPdu -> caData: " << resPdu -> caData;
            //if(strcmp(resPdu -> caData, FLUSH_DIR_OK) == 0){
            strncpy(resPdu -> caData, PRE_DIR_OK, 32);
                //qDebug() << "2 resPdu -> caData: " << resPdu -> caData;
            //}
            //else{
            //    strncpy(resPdu -> caData, PRE_DIR_FAILED, 32);
            //    qDebug() << "2 resPdu -> caData: " << resPdu -> caData;
            //}
            write((char*)resPdu, resPdu->uiPDULen);
            free(resPdu);
            resPdu = NULL;
            break;
        }


        default:
            break;

    }

    free(pdu);
    pdu = NULL;

}



void MyTcpSocket::clientOffline(){
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);
}

QString MyTcpSocket::getName(){
    return m_strName;
}
