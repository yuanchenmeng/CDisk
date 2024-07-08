#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MyTcpServer();

    static MyTcpServer &getInstance();
    virtual void incomingConnection(qintptr socketDescriptor);
    void resend(const char* pername, PDU* pdu);
    QString getStrRootPath() const;
    void setStrRootPath(const QString &strRootPath);

public slots:
    void deleteSocket(MyTcpSocket *mysocket);
private:
    QList<MyTcpSocket*> m_tcpSocketList;
    QString m_strRootPath;
};

#endif // MYTCPSERVER_H
