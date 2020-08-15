#include "globals.h"

QTranslator translator;
QTextStream sout(stdout);

QVector<QString> g_bookNames;
QVector<QString> g_booksAbbr;

QDir dataDir;
QDir userDataDir;
QDir userCfgDir;

QFile settingsFile;
