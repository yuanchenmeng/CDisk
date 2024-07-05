#include "mytcpsocket.h"
#include <QDebug>

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this, SIGNAL(readyRead()), this, SLOT(recvMsg()));
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
        default:
            break;

    }

    free(pdu);
    pdu = NULL;

}
