#ifndef COMMON_H
#define COMMON_H

#include "globals.h"
#include <QStandardPaths>
#include <QApplication>

void installTranslator();
void setDataDir(); // set app data dir e.g. /usr/share/soulanchor
void setUserDataDir(); // e.g. ~/.local/share/soulanchor/
void setUserCfgDirAndSettings(); // e.g. ~/.config/soulanchor/ soulanchor.conf

#endif // COMMON_H
