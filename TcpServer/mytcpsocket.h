#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QFile>
#include <QTimer>
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
    void handledownloadFileData();
private:
    QString m_strName;
    TransFile* m_uploadFile; // transferring file for uploading
    QFile *m_pDownloadFile; // transferring file for downloading
    QTimer *m_pTimer;
};

#endif // MYTCPSOCKET_H
