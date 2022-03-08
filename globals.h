#ifndef GLOBALS_H
#define GLOBALS_H

#include <QSettings>
#include <QTextStream>
#include <QDir>
#include <QTranslator>

extern QTextStream sout;

extern QList<QString> g_bookNames;
extern QList<QString> g_booksAbbr;

extern QDir dataDir;
extern QDir userDataDir;
extern QDir userCfgDir;

extern QTranslator translator;

extern QFile settingsFile;

#endif // GLOBALS_H
