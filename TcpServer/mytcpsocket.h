#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include "protocal.h"
#include <QTcpSocket>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
public slots:
void recvMsg(); 
};

#endif // MYTCPSOCKET_H
