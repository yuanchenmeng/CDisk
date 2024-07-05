#include "opedb.h"
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

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


bool OpeDB::handleRegist(const char* name, const char* pwd){
    if (name == NULL || pwd == NULL){return false   ;}
    QString data = QString(
                       "insert into userInfo(name, pwd) values(\'%1\', \'%2\')"
                       ).arg(name).arg(pwd);
    QSqlQuery query;
    qDebug() << "Query " << data;
    if (!query.exec(data)) {
        qDebug() << "Error: " << query.lastError().text();
        return false;
    }
    return true;
    //return query.exec(data);
}
