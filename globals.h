/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QSettings>
#include <QTextStream>
#include <QDir>
#include <QTranslator>
#include <QIcon>

extern QTextStream sout;

extern QList<QString> g_bookNames;
extern QList<QString> g_booksAbbr;

extern QDir dataDir;
extern QDir userDataDir;
extern QDir userCfgDir;

extern QTranslator translator;
extern QFile settingsFile;

#endif // GLOBALS_H
