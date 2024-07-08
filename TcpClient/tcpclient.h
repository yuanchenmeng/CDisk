#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>

#include "opewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class TcpClient;
}
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig();

    static TcpClient &getInstance();
    QTcpSocket &getTcpSocket();
    QString loginName();
public slots:
    void showConnect();
    void recvMsg();

    QString getStrCurPath() const;
    void setStrCurPath(const QString &strCurPath);
    QString getStrRootPath() const;
    void setStrRootPath(const QString &strRootPath);

private slots:
    void on_send_pb_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_cancel_pb_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;

    QTcpSocket m_tcpSocket;
    QString m_strLoginName; 
    QString m_strRootPath;
    QString m_strCurPath;
};
#endif // TCPCLIENT_H
