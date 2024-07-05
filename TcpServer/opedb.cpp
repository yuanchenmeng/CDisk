#include "opedb.h"
#include <QMessageBox>
#include <QDebug>

OpeDB::OpeDB(QObject *parent)
    : QObject{parent}
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

OpeDB& OpeDB::getInstance(){
    static OpeDB instance;
    return instance;
}


void OpeDB::init(){
    m_db.setHostName("localhost");
    m_db.setDatabaseName("E:\\CloudDisk\\TcpServer\\cloud.db");
    if (m_db.open()){
        qDebug() << " looks good?";
    }
    else{
        QMessageBox::critical(NULL, "open db", "open db failed");
    }
}

OpeDB::~OpeDB(){
    m_db.close();
}
