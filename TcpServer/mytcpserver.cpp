#include "mytcpserver.h"
#include <QDebug>


MyTcpServer::MyTcpServer() {}


MyTcpServer &MyTcpServer::getInstance() {
    static MyTcpServer instance;
    return instance;
}



void MyTcpServer::incomingConnection(qintptr socketDescriptor) {

    qDebug() << "new client connect" ;
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_tcpSocketList.append(pTcpSocket);

}
