#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include "globals.h"
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>

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
