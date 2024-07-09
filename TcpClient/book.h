#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include "protocal.h"

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(PDU* pdu);
    QString strTryEntryDir() const;
    void setStrTryEntryDir(const QString &strTryEntryDir);
    void clearStrTryEntryDir();

public slots:
    void createDir();
    void flushDir();
    void delFileOrDir(); 
    void renameFile();
    void entryDir(const QModelIndex &index);
    void returnPreDir();
    void uploadFile();
    void startTimer();
    void uploadFileData();
    void downloadFile();
    TransFile* getDownloadFileInfo();
signals:
private:
    QListWidget *m_pFileListW;     
    QPushButton *m_pReturnPrePB;   
    QPushButton *m_pCreateDirPB;     
    QPushButton *m_pRenameFilePB;  
    QPushButton *m_pFlushDirPB;    
    QPushButton *m_pUploadFilePB;  
    QPushButton *m_pDownloadFilePB;
    QPushButton *m_pDelFileOrDirPB; 
    QPushButton *m_pShareFilePB;

    QString m_strTryEntryDir;         // temp for director entry

    QString m_strUploadFilePath;    // uploading path
    QTimer *m_pTimer;               // timer, prevents two msg sticking together
    TransFile *m_downloadFile;      // The file for downloading
};

#endif // BOOK_H
