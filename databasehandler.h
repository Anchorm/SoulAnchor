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
#include <QSqlError>
#include <QHash>

// singleton, eager
class DatabaseHandler {
public:
    static DatabaseHandler& getInstance();

    QSqlDatabase& getDatabase(const QString& dbName);

    void copyDatabase(const QString& databaseName); // bibles.db from system data to user data
    // add databases to QHash databases
    void addRosterDb();
    void addBookmarksDb();
    void addBibleDb(); // translations
    void addDictDb(); // dictionaries
    void addVariousDb(); // maps, notes, images, subheadings, crossreferences, contemplations, topical
    void addBooksDb(); // bookname and abbreviations in multiple languages

private:
    QHash<QString, QSqlDatabase> databases;

    DatabaseHandler(); // Private constructor to prevent external instantiation
    ~DatabaseHandler(); // Destructor to handle cleanup

    // Disable copy constructor and assignment operator
    DatabaseHandler(const DatabaseHandler&) = delete;
    DatabaseHandler& operator=(const DatabaseHandler&) = delete;
};

#endif // DATABASEHANDLER_H
