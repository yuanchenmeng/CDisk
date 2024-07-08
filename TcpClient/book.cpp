#include "book.h"

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
}
