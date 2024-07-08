#include "tcpserver.h"
#include "./ui_tcpserver.h"
#include "mytcpserver.h"

#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    OpeDB::getInstance().init();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP), m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if (file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
        file.close();
        strData.replace("\r\n", " ");
        QStringList strList = strData.split(" ");

        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        MyTcpServer::getInstance().setStrRootPath(strList.at(2));
        qDebug() << "IP: " << m_strIP << " port: " << m_usPort 
        << " root path: " << MyTcpServer::getInstance().getStrRootPath();
    }

    else{
        QMessageBox::critical(this, "config server",  "config server failed");
    }
}
