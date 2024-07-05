#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>

class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    static OpeDB& getInstance();

signals:
};

#endif // OPEDB_H
