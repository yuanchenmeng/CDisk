#include "opewidget.h"

OpeWidget::OpeWidget(QWidget *parent)
    : QWidget{parent}

{
    m_pListW = new QListWidget(this);
    m_pListW->addItem("Friend");
    m_pListW->addItem("Libs");

    m_pFriend = new Friend;
    m_pBook = new Book;

    m_pSW = new QStackedWidget;
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pBook);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pListW);
    pMain->addWidget(m_pSW);

    setLayout(pMain);

    connect(m_pListW, SIGNAL(currentRowChanged(int)),
    m_pSW, SLOT(setCurrentIndex(int)));
}
