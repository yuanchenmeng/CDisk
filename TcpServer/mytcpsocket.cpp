#include "mytcpsocket.h"
#include <QDebug>

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
            PDU * respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
            if (ret){
                strcpy(respdu->caData, "Login is OK ! !");
                m_strName = caName;
            }
            else{strcpy(respdu->caData, "Login ff");}

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
