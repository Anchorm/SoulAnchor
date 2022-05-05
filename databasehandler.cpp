#include "databasehandler.h"

DatabaseHandler dbH;

DatabaseHandler::DatabaseHandler()
{
    // qDebug() << "these db drivers are available: " << QSqlDatabase::drivers();
}

bool DatabaseHandler::openDataBases() {
    // db's must exist or we exit
    // db's in user data dir will be created

    extraDb = QSqlDatabase::addDatabase("QSQLITE", "extra");
    extraDb.setConnectOptions("QSQLITE_OPEN_READONLY");
    extraDb.setDatabaseName(::dataDir.path() + "/db/extra.db");
    extraDb.open();

    if (!extraDb.isOpen()) {
        sout << "could not open database: extra.db in "
             << ::dataDir.path() << Qt::endl;
        return false;
    }

    bibleDb = QSqlDatabase::addDatabase("QSQLITE", "bibles");
    bibleDb.setConnectOptions("QSQLITE_OPEN_READONLY");
    bibleDb.setDatabaseName(::dataDir.path() + "/db/bibles.db");
    bibleDb.open();

    if (!bibleDb.isOpen()) {
        sout << "could not open database: bibles.db in "
             << ::dataDir.path() << Qt::endl;
        return false;
    }

    dictDb = QSqlDatabase::addDatabase("QSQLITE", "dictionaries");
    dictDb.setConnectOptions("QSQLITE_OPEN_READONLY");
    dictDb.setDatabaseName(::dataDir.path() + "/db/dictionaries.db");
    dictDb.open();

    if (!dictDb.isOpen()) {
        sout << "could not open database: dictionaries.db in "
             << ::dataDir.path() <<  Qt::endl;
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
        return false;
    }
    if (!bookmarksDb.isOpen()) {
        sout << "could not open or create database: bookmarks.db in "
             << ::userDataDir.path() <<  Qt::endl;
        return false;
    }

    return true;
}

int DatabaseHandler::getChapterCount(int bookNumber, QString tlAbbr )
{
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    if(tlAbbr == "default"){
        tlAbbr = settings.value("translation", "NET").toString();
    }
    if (tlAbbr.isEmpty()) {
        tlAbbr = "NET";
    }

    QString sql = QString("SELECT COUNT(DISTINCT c) FROM t_%1 "
                    "WHERE b = %2 ").arg(tlAbbr).arg(bookNumber);
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
        extraDb.close();
        rosterDb.close();
        bookmarksDb.close();
}
