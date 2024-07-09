#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QFile>
#include <QTcpSocket>
#include "protocal.h"
#include "opedb.h"

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString getName();
signals:
    void offline(MyTcpSocket* mysocket);
public slots:
    void recvMsg();
    void clientOffline();
private:
    QString m_strName;
    TransFile* m_uploadFile; // transferring file
};

#endif // MYTCPSOCKET_H
