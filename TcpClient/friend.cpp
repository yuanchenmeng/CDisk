#include "friend.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QDebug>

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{

    m_pShowMsgTe = new QTextEdit;
    m_pFriendListWidget = new QListWidget;
    m_pInputMsgLE = new QLineEdit;

    m_pDelFriendPB = new QPushButton("Del");
    m_pFlushFriendPB = new QPushButton("Refresh");
    m_pShowOnlineUsrPB = new QPushButton("Show Online User");
    m_pSearchUsrPB = new QPushButton("Search User");
    m_pMsgSendPB = new QPushButton("Send Info");
    m_pPrivateChatPB = new QPushButton("Chat Private");

    QVBoxLayout *pRightPBVBL = new QVBoxLayout;
    pRightPBVBL->addWidget(m_pDelFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUsrPB);
    pRightPBVBL->addWidget(m_pSearchUsrPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTe);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    m_pOnline = new Online;

    QVBoxLayout *pMain = new QVBoxLayout;
    
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();
    
    setLayout(pMain);

    connect(m_pShowOnlineUsrPB, SIGNAL(clicked(bool)), this, SLOT(showOnline()));
    connect(m_pSearchUsrPB, SIGNAL(clicked(bool)), this, SLOT(searchUsr()));
    connect(m_pFlushFriendPB, SIGNAL(clicked(bool)), this, SLOT(flushFriendList()));
    connect(m_pDelFriendPB, SIGNAL(clicked(bool)), this, SLOT(deleteFriend()));
    
}

void Friend::showOnline(){
    if (m_pOnline ->isHidden()){
        m_pOnline -> show();

        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket()
        .write((char*) pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else{m_pOnline -> hide();}
}


void Friend::showAllOnlineUsr(PDU *pdu){
    if (NULL == pdu){
        return;
    }
    m_pOnline->showUsr(pdu);

}


void Friend::searchUsr(){
    QString m_strSearchName = QInputDialog::getText(this, "Search", "search user name");
    if (!m_strSearchName.isEmpty()){
        qDebug() << "Search str: " << m_strSearchName;
        PDU *pdu = mkPDU(0);
        memcpy(pdu->caData, m_strSearchName.toStdString().c_str(), m_strSearchName.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_REQUEST;
        TcpClient::getInstance().getTcpSocket()
        .write((char*) pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

    }
}

void Friend::updateFriendList(PDU *pdu){
    if(NULL == pdu) {return;}
    uint uiSize = pdu -> uiMsgLen / 36;
    char caName[32] = {'\0'};
    char caOnline[4] = {'\0'};

    m_pFriendListWidget -> clear();
    for(uint i = 0; i < uiSize; ++ i){
        memcpy(caName, (char*)(pdu -> caMsg) + i * 36, 32);
        memcpy(caOnline, (char*)(pdu -> caMsg) + 32 + i * 36, 4);
        m_pFriendListWidget -> addItem(QString("%1\t%2").arg(caName)
            .arg(strcmp(caOnline, "1") == 0?"Online":"Offline"));
    }
}

void Friend::flushFriendList(){
    QString strName = TcpClient::getInstance().loginName();
    PDU* pdu = mkPDU(0);
    pdu -> uiMsgType = ENUM_MSG_TYPE_FLSUH_FRIEND_REQUEST;
    strncpy(pdu -> caData, strName.toStdString().c_str(), strName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}


void Friend::deleteFriend(){
    if(NULL == m_pFriendListWidget -> currentItem()){
        return;
    }

    QString friName = m_pFriendListWidget -> currentItem() -> text();
    friName = friName.split("\t")[0];
    QString loginName = TcpClient::getInstance().loginName();

    qDebug() << friName;
    PDU* pdu = mkPDU(0);
    pdu -> uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    strncpy(pdu -> caData, friName.toStdString().c_str(), 32);
    strncpy(pdu -> caData + 32, loginName.toStdString().c_str(), 32);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}