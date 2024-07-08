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
                OpeWidget::getInstance().show();
                OpeWidget::getInstance().getFriend() -> flushFriendList();
                this->hide();

                // Save Root Dir logged-in user in tcp client
                m_strRootPath = QString((char*)pdu -> caMsg);
                qDebug() << "User Root Dir" << m_strRootPath;
                m_strCurPath = m_strRootPath;

            }
            else if (0 == strcmp(pdu->caData, "Login ff")){
                QMessageBox::information(this, "Lgt", "Error");
            }
            else{
                QMessageBox::information(this, "Lgt", "SSS Error");
            }
            break;
        }


        case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:
        {
            OpeWidget::getInstance().getFriend()->
            showAllOnlineUsr(pdu);

        }


        case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:
        {
            if (0 == strcmp(pdu->caData, "Not Find")){
                QMessageBox::information(this, "User Search", "Not Find");

            }
            else if (0 == strcmp(pdu->caData, "Find Online")){
                QMessageBox::information(this, "User Search", "Find1");
            }
            else{
                QMessageBox::information(this, "User Search", "Find2");
            }
            break;
        }

        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            char sourceName[32]; 
            strncpy(sourceName, pdu -> caData + 32, 32);
            int ret = QMessageBox::information(this, "New Friend Request", 
            QString("Add %1 as Friend?").arg(sourceName), QMessageBox::Yes, QMessageBox::No);

            PDU* resPdu = mkPDU(0);
            strncpy(resPdu -> caData, pdu -> caData, 32); // Respond
            strncpy(resPdu -> caData + 32, pdu -> caData + 32, 32); // Apply
            // qDebug() << "friend request info" << resPdu -> caData << " " << resPdu -> caData + 32;
            resPdu->uiMsgType = ret == QMessageBox::Yes ? 
            ENUM_MSG_TYPE_ADD_FRIEND_AGREE : ENUM_MSG_TYPE_ADD_FRIEND_REJECT;
            m_tcpSocket.write((char*)resPdu, resPdu -> uiPDULen);
            break;
        }

        
        case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
        {
            QMessageBox::information(this, "Add Friend", pdu -> caData);
            break;
        }

        case ENUM_MSG_TYPE_ADD_FRIEND_AGREE: 
        {
            QMessageBox::information(this, "Add Friend", QString("%1 added as friend").arg(pdu -> caData));
            break;
        }

        case ENUM_MSG_TYPE_ADD_FRIEND_REJECT: 
        {
            QMessageBox::information(this, "Add Friend", QString("%1 disapproved your request!!!").arg(pdu -> caData));
            break;
        }

        case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
        {
            OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
            break;
        }

        case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:
        {
            QMessageBox::information(this, "DEL Friend", pdu -> caData);
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
        {
            char sourceName[32];
            strncpy(sourceName, pdu -> caData + 32, 32);
            QMessageBox::information(this, "DEL Friend", QString("%1 deleted connection").arg(sourceName));
            break;
        }

        case ENUM_MSG_TYPE_CREATE_DIR_RESPOND: 
        {
            QMessageBox::information(this, "Create Folder", pdu -> caData);
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
        m_strLoginName = strName;
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

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}


QTcpSocket &TcpClient::getTcpSocket(){
    return m_tcpSocket;
}

QString TcpClient::loginName(){
    return m_strLoginName;
}

QString TcpClient::getStrRootPath() const{
    return m_strRootPath;
}

void TcpClient::setStrRootPath(const QString &strRootPath){
    m_strRootPath = strRootPath;
}

QString TcpClient::getStrCurPath() const{
    return m_strCurPath;
}

void TcpClient::setStrCurPath(const QString &strCurPath){
    m_strCurPath = strCurPath;
}