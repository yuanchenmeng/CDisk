#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocal.h"

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(PDU* pdu);
public slots:
    void createDir();
    void flushDir();   
signals:
private:
    QListWidget *m_pFileListW;     
    QPushButton *m_pReturnPrePB;   
    QPushButton *m_pCreateDirPB;   
    QPushButton *m_pDeleteDirPB;   
    QPushButton *m_pRenameFilePB;  
    QPushButton *m_pFlushDirPB;    
    QPushButton *m_pUploadFilePB;  
    QPushButton *m_pDownloadFilePB;
    QPushButton *m_pDeleteFilePB;  
    QPushButton *m_pShareFilePB;   
};

#endif // BOOK_H
