#include "book.h"
#include <QInputDialog>
#include <QMessageBox>
#include "tcpclient.h"

Book::Book(QWidget *parent)
    : QWidget{parent}
{
    m_pFileListW = new QListWidget;

    m_pReturnPrePB = new QPushButton("Back");
    m_pCreateDirPB = new QPushButton("Create New Folder");
    m_pDeleteDirPB = new QPushButton("Del Folder");
    m_pFlushDirPB = new QPushButton("Refresh List");

    QHBoxLayout *pDirOpVBL = new QHBoxLayout;
    pDirOpVBL -> addWidget(m_pReturnPrePB);
    pDirOpVBL -> addWidget(m_pCreateDirPB);
    pDirOpVBL -> addWidget(m_pDeleteDirPB);
    pDirOpVBL -> addWidget(m_pFlushDirPB);

    m_pRenameFilePB = new QPushButton("Rename File");
    m_pUploadFilePB = new QPushButton("Upload File");
    m_pDownloadFilePB = new QPushButton("Download File");
    m_pDeleteFilePB = new QPushButton("Del File");
    m_pShareFilePB = new QPushButton("Share File");

    QHBoxLayout *pFileOpVBL = new QHBoxLayout;
    pFileOpVBL -> addWidget(m_pRenameFilePB);
    pFileOpVBL -> addWidget(m_pUploadFilePB);
    pFileOpVBL -> addWidget(m_pDownloadFilePB);
    pFileOpVBL -> addWidget(m_pDeleteFilePB);
    pFileOpVBL -> addWidget(m_pShareFilePB);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL -> addLayout(pDirOpVBL);
    pMainVBL -> addLayout(pFileOpVBL);
    pMainVBL -> addWidget(m_pFileListW);

    setLayout(pMainVBL);

    
    connect(m_pCreateDirPB, SIGNAL(clicked(bool)), this, SLOT(createDir()));
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
