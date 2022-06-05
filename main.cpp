 /******************************************************
    SoulAnchor - X11 Bible reading tool
    by Anchorman - soulanchor at protonmail dot com

    this hope we have as an anchor of the soul
    a hope both sure and steadfast
    and one which enters within the veil
    (Hebrews 6:19)

 *******************************************/

#include <QApplication>
#include "setup.h"
#include "mainwindow.h"
#include "globals.h"
#include "databasehandler.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationVersion("1.06");

    QCommandLineParser parser;
    parser.setApplicationDescription("SoulAnchor - X11 Bible reading tool");
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    setDataDir();
    setUserDataDir();
    if (!dbH.openDataBases()) {
        exit(1);
    }
    setUserCfgDirAndSettings();
    installTranslator();

    MainWindow mainW;
    mainW.show();

    return app.exec();
}
