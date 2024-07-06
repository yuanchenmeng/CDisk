#include "tcpclient.h"
#include "./ui_tcpclient.h"
#include "protocal.h"

#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();

    connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(showConnect()));
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(recvMsg()));


    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usPort);


}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config");
    if (file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
        file.close();
        strData.replace("\r\n", " ");
        QStringList strList = strData.split(" ");

        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();

        //qDebug() << "ip: " << m_strIP << " " << m_usPort;
    }

    else{
        QMessageBox::critical(this, "config",  "config failed");
    }
}

void TcpClient::showConnect()
{
    QMessageBox::information(this, "Connect", "Connect Sucessfull");

}

void TcpClient::recvMsg()
{
    
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU * pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));
    //qDebug() << pdu->uiMsgType << " --> " << (char*) pdu -> caMsg;

    switch (pdu->uiMsgType){
        case ENUM_MSG_TYPE_REGISTER_RESPOND:
        {
            if (0 == strcmp(pdu->caData, "Register is OK ! !")){
                QMessageBox::information(this, "Regis", "Register Sucessful");
            }
            else if (0 == strcmp(pdu->caData, "Register ff")){
                QMessageBox::information(this, "Regis", "Error");
            }
            else{
                QMessageBox::information(this, "Regis", "SSS Error");
            }
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_RESPOND:
        {
            if (0 == strcmp(pdu->caData, "Login is OK ! !")){
                QMessageBox::information(this, "Lgt", "Login Sucessful");
            }
            else if (0 == strcmp(pdu->caData, "Login ff")){
                QMessageBox::information(this, "Lgt", "Error");
            }
            else{
                QMessageBox::information(this, "Lgt", "SSS Error");
            }
            break;
        }
        default:
            break;
    }

    free(pdu);
    pdu = NULL;

}

void TcpClient::on_send_pb_clicked()
{
    QString strMsg = ui->lineEdit->text();
    qDebug() << "Msg Got: " << strMsg;
    if (!strMsg.isEmpty()){
        PDU* pdu = mkPDU(strMsg.size());
        pdu->uiMsgType = 8888;
        memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.size());
        m_tcpSocket.write((char*) pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else{
        QMessageBox::warning(this, "info send", "info send can't be empty");
    }
}


void TcpClient::on_login_pb_clicked()
{

    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if (!strName.isEmpty() && ! strPwd.isEmpty()){
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);

        m_tcpSocket.write((char*) pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

    }
    else{
        QMessageBox::critical(this, "Login", "Login F1");
    }

}


void TcpClient::on_regist_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if (!strName.isEmpty() && ! strPwd.isEmpty()){
        
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGISTER_REQUEST;
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);

        m_tcpSocket.write((char*) pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

    }
    else{
        QMessageBox::critical(this, "Regis", "Regis F1");
    }

}


void TcpClient::on_cancel_pb_clicked()
{

}

