#include "databasehandler.h"

DatabaseHandler dbH;

// TODO: open database connections for app data dir db's, others on demand. OR don't bother
DatabaseHandler::DatabaseHandler()
{
    // qDebug() << "these db drivers are available: " << QSqlDatabase::drivers();
}

bool DatabaseHandler::openDataBases() {
    // db's in app data dir must exist or we exit
    // db's in user data dir will be autocreated if possible
    bibleDb = QSqlDatabase::addDatabase("QSQLITE", "bibles");
    bibleDb.setConnectOptions("QSQLITE_OPEN_READONLY");
    bibleDb.setDatabaseName(::dataDir.path() + "/db/bibles.db");
    bibleDb.open();

    if (!bibleDb.isOpen()) {
        sout << "could not open database: bibles.db in "
             << ::dataDir.path() << Qt::endl;
        sout << "must exit" << Qt::endl;
        return false;
    }

    dictDb = QSqlDatabase::addDatabase("QSQLITE", "dictionaries");
    dictDb.setConnectOptions("QSQLITE_OPEN_READONLY");
    dictDb.setDatabaseName(::dataDir.path() + "/db/dictionaries.db");
    dictDb.open();

    if (!dictDb.isOpen()) {
        sout << "could not open database: dictionaries.db in "
             << ::dataDir.path() <<  Qt::endl;
        sout << "must exit" << Qt::endl;
        return false;
    }

    devotionsDb = QSqlDatabase::addDatabase("QSQLITE", "devotions");
    devotionsDb.setConnectOptions("QSQLITE_OPEN_READONLY");
    devotionsDb.setDatabaseName(::dataDir.path() + "/db/devotions.db");
    devotionsDb.open();

    if (not devotionsDb.open()) {
        sout << "could not open database: devotions.db in "
             << ::dataDir.path() << Qt::endl;
        sout << "must exit" << Qt::endl;
        return false;
    }

    ::userDataDir.mkdir(::userDataDir.path() + "/db");

    rosterDb = QSqlDatabase::addDatabase("QSQLITE", "roster");
    rosterDb.setDatabaseName(::userDataDir.path() + "/db/roster.db");
    rosterDb.open();

    bookmarksDb = QSqlDatabase::addDatabase("QSQLITE", "bookmarks");
    bookmarksDb.setDatabaseName(::userDataDir.path() + "/db/bookmarks.db");
    bookmarksDb.open();

    QString sqlCreateTable = QString("CREATE TABLE IF NOT EXISTS 'bookmarks' "
                                "(name TEXT NOT NULL UNIQUE, "
                                "book INTEGER NOT NULL, "
                                "chapter1 INTEGER NOT NULL, "
                                "chapter2 INTEGER NOT NULL, "
                                "verse1 INTEGER NOT NULL, "
                                "verse2 INTEGER NOT NULL) "
                                );

    QSqlQuery queryAdd(sqlCreateTable, bookmarksDb);
    queryAdd.exec();

    if (!rosterDb.isOpen()) {
        sout << "could not open or create database: roster.db in "
             << ::userDataDir.path() <<  Qt::endl;
        sout << "continue anyway without roster functionality" << Qt::endl;
    }
    if (!bookmarksDb.isOpen()) {
        sout << "could not open or create database: bookmarks.db in "
             << ::userDataDir.path() <<  Qt::endl;
        sout << "continue anyway without bookmarks functionality" << Qt::endl;
    }

    return true;
}

int DatabaseHandler::getFinalChapter(int bookNumber, QString tlAbbr )
{
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    if(tlAbbr == "default"){
        tlAbbr = settings.value("translation").toString();
    }

    QString sql = QString("SELECT c FROM t_%1 WHERE b = %2 ORDER BY c "
                          "DESC LIMIT 1").arg(tlAbbr).arg(bookNumber);
    QSqlQuery query(sql, bibleDb);

    int finalChapter = 0;
    while (query.next()) {
        finalChapter = query.value(0).toInt();
    }
    return finalChapter;
}


DatabaseHandler::~DatabaseHandler() {
        bibleDb.close();
        dictDb.close();
        devotionsDb.close();
        rosterDb.close();
        bookmarksDb.close();
}
