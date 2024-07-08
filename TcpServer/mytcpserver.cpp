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

    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket*)), this, SLOT(deleteSocket(MyTcpSocket*)));
}



void MyTcpServer::deleteSocket(MyTcpSocket* mysocket) {
    qDebug() << "A client disconnecting";

    qDebug() << "User List: \n";
    for (int i = 0; i < m_tcpSocketList.size(); i++){
        qDebug() << m_tcpSocketList.at(i)->getName();
    }
    qDebug() << "EOL \n";

    QList<MyTcpSocket*>::iterator iter = m_tcpSocketList.begin();
    for (; iter != m_tcpSocketList.end(); iter++){
        if (mysocket == *iter){
            m_tcpSocketList.erase(iter);
            //(*iter) -> deleteLater();
            delete *iter;
            *iter = NULL;
            break;
        }
    }
}

void MyTcpServer::resend(const char* pername, PDU* pdu){
    if (NULL == pername || NULL == pdu){return;}
    QString strName = pername; 
    for (int i = 0; i < m_tcpSocketList.size(); i++){
        if (strName == m_tcpSocketList.at(i)->getName()){
            m_tcpSocketList.at(i)->write(
                (char*)pdu, pdu->uiPDULen
            );
            break;
        }

    }
}


QString MyTcpServer::getStrRootPath() const{
    return m_strRootPath;
}

void MyTcpServer::setStrRootPath(const QString &strRootPath){
    m_strRootPath = strRootPath;
}