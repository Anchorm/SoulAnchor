/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

#include "databasehandler.h"

DatabaseHandler::DatabaseHandler()
{
    // qDebug() << "these db drivers are available: " << QSqlDatabase::drivers();
    copyDatabase("bibles");
    // copyDatabase("various");
    // copyDatabase("dictionaries");
    addRosterDb();
    addBookmarksDb();
    addBibleDb();
    addDictDb();
    addVariousDb();
    addBooksDb();
}

DatabaseHandler& DatabaseHandler::getInstance()
{
    static DatabaseHandler instance;
    return instance;
}

void DatabaseHandler::copyDatabase(const QString& databaseName)
{
    // copy database from datadir to userdir if non existing
    // for bibles.db only atm, perhaps in a later version also for dictionaries.db, various.db
    QString source_path = ::dataDir.path() + "/db/" + databaseName + ".db";
    QString destination_path = ::userDataDir.path() + "/db/" + databaseName + ".db";

    QFile dbFile(destination_path);
    if (!dbFile.exists()) {
        if (QFile::copy(source_path, destination_path)) {
            dbFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner
                                  | QFileDevice::ReadGroup | QFileDevice::ReadOther);

            ::sout << databaseName << ".db copied to " << destination_path << Qt::endl;
        } else {
            ::sout << "could not copy " << databaseName << ".db to " << destination_path << Qt::endl;
            exit(1);
        }
    }
}

void DatabaseHandler::addRosterDb()
{
    // ::sout << __PRETTY_FUNCTION__ << Qt::endl;
    QSqlDatabase rosterDb = QSqlDatabase::addDatabase("QSQLITE", "roster");
    rosterDb.setDatabaseName(::userDataDir.path() + "/db/roster.db");
    rosterDb.open();

    if (!rosterDb.isOpen()) {
        ::sout << "could not open or create database: roster.db in "
             << ::userDataDir.path() << "/db" <<  Qt::endl;
        ::sout << rosterDb.lastError().text() << Qt::endl;
        exit(1);
    } else {
        databases.insert("roster", rosterDb);
    }
}

void DatabaseHandler::addBookmarksDb()
{
    QSqlDatabase bookmarksDb = QSqlDatabase::addDatabase("QSQLITE", "bookmarks");
    bookmarksDb.setDatabaseName(::userDataDir.path() + "/db/bookmarks.db");

    bookmarksDb.open();
    if (!bookmarksDb.isOpen()) {
        ::sout << "could not open or create database: bookmarks.db in "
             << ::userDataDir.path() << "/db" <<  Qt::endl;
        ::sout << bookmarksDb.lastError().text() << Qt::endl;
        exit(1);
    } else {
        databases.insert("bookmarks", bookmarksDb);
    }
}

QSqlDatabase& DatabaseHandler::getDatabase(const QString& dbName)
{
    if (!databases.contains(dbName)) {
        ::sout << dbName << " does not exist in QHash databases. Exiting!" << Qt::endl;
        exit(1);
    }

    return databases[dbName];
}

void DatabaseHandler::addBibleDb()
{
    QSqlDatabase bibleDb = QSqlDatabase::addDatabase("QSQLITE", "bibles");
    bibleDb.setDatabaseName(::userDataDir.path() + "/db/" + "bibles" + ".db");
    if (!bibleDb.open()) {
        ::sout << "Could not open database: "
               << ::userDataDir.path() << "/db/"
               << "bibles" << ".db" << Qt::endl;
        ::sout << bibleDb.lastError().text() << Qt::endl;
        exit(1);
    }
    databases.insert("bibles", bibleDb);
}

void DatabaseHandler::addDictDb()
{
    QSqlDatabase dictDb = QSqlDatabase::addDatabase("QSQLITE", "dictionaries");
    dictDb.setDatabaseName(::dataDir.path() + "/db/" + "dictionaries" + ".db");
    if (!dictDb.open()) {
        ::sout << "Could not open database: "
               << ::dataDir.path() << "/db/"
               << "dictionaries" << ".db" << Qt::endl;
        ::sout << dictDb.lastError().text() << Qt::endl;
        exit(1);
    }
    databases.insert("dictionaries", dictDb);
}

void DatabaseHandler::addVariousDb()
{
    QSqlDatabase varDb = QSqlDatabase::addDatabase("QSQLITE", "various");
    varDb.setDatabaseName(::dataDir.path() + "/db/" + "various" + ".db");
    if (!varDb.open()) {
        ::sout << "Could not open database: "
               << ::dataDir.path() << "/db/"
               << "various" << ".db" << Qt::endl;
        ::sout << varDb.lastError().text() << Qt::endl;
        exit(1);
    }
    databases.insert("various", varDb);
}

void DatabaseHandler::addBooksDb()
{
    QSqlDatabase booksDb = QSqlDatabase::addDatabase("QSQLITE", "books");
    booksDb.setDatabaseName(::dataDir.path() + "/db/" + "books-i18n" + ".db");
    if (!booksDb.open()) {
        ::sout << "Could not open database: "
               << ::dataDir.path() << "/db/"
               << "books-i18n" << ".db" << Qt::endl;
        ::sout << booksDb.lastError().text() << Qt::endl;
        exit(1);
    }
    databases.insert("books", booksDb);
}

DatabaseHandler::~DatabaseHandler()
{
    for (auto& db : databases)
        db.close();
}
