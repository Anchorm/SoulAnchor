/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

#ifndef SETUP_H
#define SETUP_H

#include "globals.h"

#include <QTranslator>
#include <QStandardPaths>
#include <QApplication>
#include <QMessageBox>

void setDataDir(); // set app data dir e.g. /usr/share/soulanchor
void setUserDataDir(); // e.g. ~/.local/share/soulanchor/
void setUserCfgDirAndSettings(); // e.g. ~/.config/soulanchor/ soulanchor.conf

class LanguageManager {
private:
public:
    static QTranslator m_translator;
    static void changeLanguage(const QString& langCode);
    LanguageManager(const QTranslator &translator);
};

#endif
