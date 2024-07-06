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
        qDebug() << "DB Init looks good?, m_db is open";
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


bool OpeDB::handleLogin(const char* name, const char* pwd){
    if (name == NULL || pwd == NULL){return false   ;}
    QString data = QString(
                       "select * from userInfo where name = \'%1\' and pwd = \'%2\' and online = 0"
                       ).arg(name).arg(pwd);
    QSqlQuery query;
    qDebug() << "DBCMD: Online Set " << data;

    if (!query.exec(data)) {
        qDebug() << "Error: " << query.lastError().text();
        return false;
    }

    if (query.next()){
        data = QString("update userInfo set online = 1 where name = \'%1\' and pwd = \'%2\' ")
        .arg(name).arg(pwd);
        QSqlQuery query;
        query.exec(data);
        return true;
    }
    else{return false;}
}


void OpeDB::handleOffline(const char* name){
    if (name == NULL){return;}
    QString data = QString(
                       "update userInfo set online = 0 where name = \'%1\'"
                       ).arg(name);
    QSqlQuery query;
    qDebug() << "DBCMD: Offline Set " << data;
    if (!query.exec(data)) {
        qDebug() << "Error: " << query.lastError().text();
        return;
    }
    return;
}


QStringList OpeDB::handleAllOnline(){
    QString data = QString("select name from userInfo where online = 1");
    QSqlQuery query;
    qDebug() << "DBCMD: All online " << data;
    query.exec(data);
    QStringList result;
    result.clear();
    while (query.next()){
        result.append(query.value(0).toString());
    }
    return result;
}