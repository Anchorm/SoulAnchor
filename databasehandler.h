#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include "globals.h"
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>

// TODO: getters/setters OR don't bother since all we need are globals

class DatabaseHandler {
public:
    QSqlDatabase bibleDb;
    QSqlDatabase rosterDb;
    QSqlDatabase bookmarksDb;
    QSqlDatabase dictDb;
    QSqlDatabase extraDb;
    bool openDataBases();
    int getChapterCount(int bookNumber, QString tlAbbr = "default");
    DatabaseHandler();
    ~DatabaseHandler();
};

extern DatabaseHandler dbH;

#endif // DATABASEHANDLER_H
