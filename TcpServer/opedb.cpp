#include "opedb.h"

OpeDB::OpeDB(QObject *parent)
    : QObject{parent}
{}

OpeDB& OpeDB::getInstance(){
    static OpeDB instance;
    return instance;
}

