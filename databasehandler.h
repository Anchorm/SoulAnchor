/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

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
