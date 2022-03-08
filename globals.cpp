#include "globals.h"

QTranslator translator;

QTextStream sout(stdout);

QList<QString> g_bookNames;
QList<QString> g_booksAbbr;

QDir dataDir;
QDir userDataDir;
QDir userCfgDir;

QFile settingsFile;
