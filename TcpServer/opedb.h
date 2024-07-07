#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    static OpeDB& getInstance();
    void init();
    ~ OpeDB();

    bool handleRegist(const char* name, const char* pwd);
    bool handleLogin(const char* name, const char* pwd);
    void handleOffline(const char* name);

    QStringList handleAllOnline();
    int handleSearchUsr(const char* name);

    int handleAddFriend(const char *addedName, const char *sourceName);
    bool handleAddFriendAgree(const char *addedName, const char *sourceName); 
    int getIdByUserName(const char *name);
    QStringList handleFlushFriendRequest(const char *name);

signals:
public slots:
private:
    QSqlDatabase m_db;
};

#endif // OPEDB_H
