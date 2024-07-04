#include "tcpclient.h"
#include "./ui_tcpclient.h"

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
