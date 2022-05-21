/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

#include "globals.h"

QTranslator translator;

QTextStream sout(stdout);

QList<QString> g_bookNames;
QList<QString> g_booksAbbr;

QDir dataDir;
QDir userDataDir;
QDir userCfgDir;

QFile settingsFile;
