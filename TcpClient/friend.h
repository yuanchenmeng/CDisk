#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "online.h"
#include "protocal.h"


class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void showAllOnlineUsr(PDU *pdu);
    QString m_strSearchName;
    void setOnlineUsers(PDU* pdu);
    void updateFriendList(PDU *pdu);
    

signals:
public slots:
    void showOnline();
    void searchUsr();
    void flushFriendList();
    void deleteFriend();  
private:
    QTextEdit *m_pShowMsgTe;
    QListWidget *m_pFriendListWidget;
    QLineEdit * m_pInputMsgLE;

    QPushButton *m_pDelFriendPB;
    QPushButton *m_pFlushFriendPB;
    QPushButton *m_pShowOnlineUsrPB;
    QPushButton *m_pSearchUsrPB;
    QPushButton *m_pMsgSendPB;
    QPushButton *m_pPrivateChatPB;

    Online *m_pOnline;
    
};

#endif // FRIEND_H
