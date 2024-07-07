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

int OpeDB::handleSearchUsr(const char* name){
    if (name == NULL){return -1;}
    QString data = QString("select online from userInfo where name = \'%1\'").arg(name);
    QSqlQuery query;
    qDebug() << "DBCMD: Search " << data;
    if (!query.exec(data)) {
        qDebug() << "Error: " << query.lastError().text();
        return -1;
    }
    if (query.next()){
        int ret = query.value(0).toInt();
        return ret == 1 ? 1 : 0;
    }
    return -1;
}


int OpeDB::handleAddFriend(const char* addedName, const char* sourceName){
    if(NULL == addedName || NULL == sourceName){return 4;}
    QString data = 
    QString("select * from friend where (id = (select id from userInfo where name = \'%1\') and friendId = (select id from userInfo where name = \'%2\')) or " 
    "(id = (select id from userInfo where name = \'%3\') and "
    "friendId = (select id from userInfo where name = \'%4\'))")
            .arg(sourceName).arg(addedName).arg(addedName).arg(sourceName);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
        return 3;
    }
    else{
        return handleSearchUsr(addedName);
    }
}

bool OpeDB::handleAddFriendAgree(const char *addedName, const char *sourceName)
{
    if(NULL == addedName || NULL == sourceName){
        //qDebug() << "handleAddFriendAgree: name is NULL";
        return false;
    }

    int sourceUserId = -1; 
    int addedUserId = -1;


    addedUserId = getIdByUserName(addedName);
    sourceUserId = getIdByUserName(sourceName);

    QString strQuery = QString("insert into friendInfo values(%1, %2) ").arg(sourceUserId).arg(addedUserId);
    QSqlQuery query;

    qDebug() << "DBCMD: handleAddFriendAgree " << strQuery;

    return query.exec(strQuery);
}

int OpeDB::getIdByUserName(const char *name)
{
    if(NULL == name){return -1;}

    QString strQuery = QString("select id from userInfo where name = \'%1\' ").arg(name);
    QSqlQuery query;

    query.exec(strQuery);
    if(query.next()){
        return query.value(0).toInt();
    }
    return -1;

}


QStringList OpeDB::handleFlushFriendRequest(const char *name){
    QStringList strFriendList;
    strFriendList.clear();
    if (NULL == name){return strFriendList;}

    QString strQuery = QString("select id from userInfo where name = \'%1\' and online = 1 ").arg(name);
    QSqlQuery query;
    int iId = -1;
    query.exec(strQuery);
    if (query.next()){
        iId = query.value(0).toInt();
    }

    strQuery = QString("select name, online from userInfo where id in "
                       "((select friendId from friendinfo "
                       "where id = %1) union (select id from friendinfo "
                       "where friendId = %2))").arg(iId).arg(iId);
    query.exec(strQuery);
    while(query.next()){
        char friName[32];
        char friOnline[4];
        strncpy(friName, query.value(0).toString().toStdString().c_str(), 32);
        strncpy(friOnline, query.value(1).toString().toStdString().c_str(), 4);
        strFriendList.append(friName);
        strFriendList.append(friOnline);
    }

    return strFriendList;
}