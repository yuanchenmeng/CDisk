#include "book.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QFileDialog>
#include "tcpclient.h"

Book::Book(QWidget *parent)
    : QWidget{parent}
{
    m_strTryEntryDir.clear(); // clear temp path variable

    m_pFileListW = new QListWidget;

    m_pTimer = new QTimer;

    m_pReturnPrePB = new QPushButton("Back");
    m_pCreateDirPB = new QPushButton("Create New Folder");
    m_pDelFileOrDirPB = new QPushButton("Del Folder/File");
    m_pFlushDirPB = new QPushButton("Refresh List");

    QHBoxLayout *pDirOpVBL = new QHBoxLayout;
    pDirOpVBL -> addWidget(m_pReturnPrePB);
    pDirOpVBL -> addWidget(m_pCreateDirPB);
    pDirOpVBL -> addWidget(m_pDelFileOrDirPB);
    pDirOpVBL -> addWidget(m_pFlushDirPB);

    m_pRenameFilePB = new QPushButton("Rename File");
    m_pUploadFilePB = new QPushButton("Upload File");
    m_pDownloadFilePB = new QPushButton("Download File");
    m_pShareFilePB = new QPushButton("Share File");

    QHBoxLayout *pFileOpVBL = new QHBoxLayout;
    pFileOpVBL -> addWidget(m_pRenameFilePB);
    pFileOpVBL -> addWidget(m_pUploadFilePB);
    pFileOpVBL -> addWidget(m_pDownloadFilePB);
    pFileOpVBL -> addWidget(m_pShareFilePB);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL -> addLayout(pDirOpVBL);
    pMainVBL -> addLayout(pFileOpVBL);
    pMainVBL -> addWidget(m_pFileListW);

    setLayout(pMainVBL);

    connect(m_pCreateDirPB, SIGNAL(clicked(bool)), this, SLOT(createDir()));
    connect(m_pFlushDirPB, SIGNAL(clicked(bool)), this, SLOT(flushDir()));
    connect(m_pDelFileOrDirPB, SIGNAL(clicked(bool)), this, SLOT(delFileOrDir()));
    connect(m_pRenameFilePB, SIGNAL(clicked(bool)), this, SLOT(renameFile()));
    connect(m_pFileListW, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(entryDir(QModelIndex)));
    connect(m_pReturnPrePB, SIGNAL(clicked(bool)), this, SLOT(returnPreDir()));
    connect(m_pUploadFilePB, SIGNAL(clicked(bool)), this, SLOT(uploadFile()));
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(uploadFileData()));
}

void Book::createDir(){
    QString strDirName = QInputDialog::getText(this, "New Folder", "Folder Name:"); 
    QString strCurPath = TcpClient::getInstance().getStrCurPath();

    if(strDirName.isEmpty()){
        QMessageBox::warning(this, "New Folder", "Folder name can't be empty");
        return ;
    }
    PDU *pdu = mkPDU(strCurPath.size() + 1);
    pdu -> uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
    strncpy(pdu -> caData, strDirName.toStdString().c_str(), strDirName.size());
    memcpy((char*)pdu ->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}


void Book::flushDir()
{
    QString strCurPath = TcpClient::getInstance().getStrCurPath();
    PDU *pdu = mkPDU(strCurPath.size() + 1);
    pdu -> uiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_REQUEST;
    memcpy((char*)pdu ->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::updateFileList(PDU *pdu){
    if(NULL == pdu){return;}
    uint uiFileNum = pdu -> uiMsgLen / sizeof(FileInfo); 
    FileInfo *pFileInfo = NULL; 
    QListWidgetItem *pItem = NULL;
    m_pFileListW -> clear();

    for(uint i = 0; i < uiFileNum; ++ i){
        pFileInfo = (FileInfo*)(pdu -> caMsg) + i;
        if(strcmp(pFileInfo -> caName, ".") == 0 || strcmp(pFileInfo -> caName, "..") == 0)
        {   // Not show "." or ".."
            continue;
        }
        pItem = new QListWidgetItem;

        if(pFileInfo ->bIsDir){
            pItem->setIcon(QIcon(QPixmap(":/icon/dir.jpeg")));
        }
        else{
            pItem->setIcon(QIcon(QPixmap(":/icon/file.jpeg")));
        }
        pItem ->setText(QString("%1\t%2\t%3").arg(pFileInfo->caName)
            .arg(pFileInfo->uiSize).arg(pFileInfo->caTime));
        m_pFileListW->addItem(pItem);
    }
}

void Book::delFileOrDir(){
    QString strCurPath = TcpClient::getInstance().getStrCurPath();
    QListWidgetItem *qItem = m_pFileListW->currentItem();
    if(NULL == qItem){
        QMessageBox::warning(this, "DEL File", "Please select the file to delete!");
        return;
    }
    QString strFileName = qItem->text().split('\t')[0];
    QString strDelPath = QString("%1/%2").arg(strCurPath).arg(strFileName);
    qDebug() << "DEL obj from path: " << strDelPath;
    PDU *pdu = mkPDU(strDelPath.size() + 1);

    pdu -> uiMsgType = ENUM_MSG_TYPE_DELETE_FILE_REQUEST;
    memcpy((char*)pdu ->caMsg, strDelPath.toStdString().c_str(), strDelPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}


QString Book::strTryEntryDir() const{
    return m_strTryEntryDir;
}

void Book::setStrTryEntryDir(const QString &strTryEntryDir){
    m_strTryEntryDir = strTryEntryDir;
}


void Book::renameFile(){
    QString strCurPath = TcpClient::getInstance().getStrCurPath();
    QListWidgetItem *qItem = m_pFileListW->currentItem(); // Read current select file/dir 
    if(NULL == qItem){
        QMessageBox::warning(this, "Rename Ops", "Pleaase select the file/dir you want to rename!");
        return;
    }
    QString strOldName = qItem -> text().split('\t')[0]; 
    QString strNewName = QInputDialog::getText(this, "Ops", "Enter New Name"); 
    qDebug() << "Client Rename Ops:" << strCurPath << " " << strOldName << " -> " << strNewName;

    if(strNewName.isEmpty()){
        QMessageBox::warning(this, "Ops", "Name can't be empty");
        return ;
    }

    PDU *pdu = mkPDU(strCurPath.size() + 1);
    pdu -> uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
    strncpy(pdu -> caData, strOldName.toStdString().c_str(), 32);
    strncpy(pdu -> caData + 32, strNewName.toStdString().c_str(), 32);
    memcpy((char*)pdu ->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::entryDir(const QModelIndex &index){
    QString strCurPath = TcpClient::getInstance().getStrCurPath();
    QString strFileName = index.data().toString();
    strFileName = strFileName.split('\t')[0];
    QString strEntryPath = QString("%1/%2").arg(strCurPath).arg(strFileName);
    qDebug() << "Client Select as new entry: " << strEntryPath;
    m_strTryEntryDir = strEntryPath;

    PDU* pdu = mkPDU(strEntryPath.size() + 1);
    pdu -> uiMsgType = ENUM_MSG_TYPE_ENTRY_DIR_REQUEST;
    memcpy((char*)pdu -> caMsg, strEntryPath.toStdString().c_str(), strEntryPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::clearStrTryEntryDir(){
    m_strTryEntryDir.clear();
}


void Book::returnPreDir(){
    QString strCurPath = TcpClient::getInstance().getStrCurPath();
    QString strRootPath = TcpClient::getInstance().getStrRootPath();

    if(strCurPath == strRootPath){
        QMessageBox::warning(this, "Change Dir: cd ..", "Root Dir can't go back");
        return ;
    }
    int index = strCurPath.lastIndexOf("/");
    strCurPath = strCurPath.remove(index, strCurPath.size() - index);
    qDebug() << "Back to " << strCurPath;
    m_strTryEntryDir = strCurPath;

    PDU* pdu = mkPDU(strCurPath.size() + 1);
    pdu -> uiMsgType = ENUM_MSG_TYPE_PRE_DIR_REQUEST;
    memcpy((char*)pdu -> caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}



void Book::uploadFile(){
    QString strCurPath = TcpClient::getInstance().getStrCurPath(); 
    m_strUploadFilePath = QFileDialog::getOpenFileName();
    qDebug() << m_strUploadFilePath;

    if(m_strUploadFilePath.isEmpty()){
        QMessageBox::warning(this, "Uploading File", "Please select a file to upload");
        return;
    }

    int index = m_strUploadFilePath.lastIndexOf('/');
    QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size() - index - 1);

    QFile file(m_strUploadFilePath);
    qint64 fileSize = file.size();

    qDebug() << "Uploading Handle, uploading file size: " << strFileName << " " << fileSize;
    PDU* pdu = mkPDU(strCurPath.size() + 1);
    pdu -> uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
    memcpy(pdu -> caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
    sprintf(pdu -> caData, "%s %lld", strFileName.toStdString().c_str(), fileSize);

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::startTimer(){  m_pTimer -> start(1000); }

void Book::uploadFileData(){
    m_pTimer->stop();

    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly)){ // open failed
        QMessageBox::warning(this, "Uploading File", "Failed to open the file");
    }

    char *pBuffer = new char[4096]; // 4096 is more efficient for reading & writing
    qint64 iActualSize = 0;         // actual reading size

    while(true){
        iActualSize = file.read(pBuffer, 4096);
        if (iActualSize > 0 && iActualSize <= 4096){
            TcpClient::getInstance().getTcpSocket().write(pBuffer, iActualSize);
        }
        else if (iActualSize == 0){
            break;
        }
        else{
            QMessageBox::warning(this, "Uploading File", "Uploading Failed");
            break;
        }
    }
    file.close();
    delete [] pBuffer;
    pBuffer = NULL;
    m_strUploadFilePath.clear();
}