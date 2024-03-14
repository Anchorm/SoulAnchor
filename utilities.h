#ifndef UTILITIES_H
#define UTILITIES_H

#include "globals.h"
#include "databasehandler.h"

namespace Utilities {

inline bool handleQuery(QSqlQuery& query)
{
    if (!query.exec()) {
        ::sout << "Error executing query:" << query.lastError().text();
        return false;
    }
    return true;
}

inline int getChapterCount(int bookNumber, QString tlAbbr = "default")
{
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);

    if(tlAbbr == "default" or tlAbbr.isEmpty())
        tlAbbr = settings.value("translation", "net").toString();

    DatabaseHandler& dbH = DatabaseHandler::getInstance();
    QSqlDatabase& bibleDb = dbH.getDatabase("bibles");

    QString sql = QString("SELECT COUNT(DISTINCT c) FROM t_" + tlAbbr +
                          " WHERE b = ?");
    QSqlQuery query(bibleDb);
    query.prepare(sql);
    query.addBindValue(bookNumber);

    if (query.exec() && query.next()) {
        int finalChapter = query.value(0).toInt();
        return finalChapter;
    } else {
        ::sout << "Error executing SQL query finalChapter: " << query.lastError().text();
        return 1;
    }
}

}

#endif // UTILITIES_H
