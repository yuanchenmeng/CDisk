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
    connect(m_pSearchUsrPB, SIGNAL(clicked(bool)),
    this, SLOT(searchUsr()));
    
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