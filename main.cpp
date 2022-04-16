 /******************************************************
    SoulAnchor - X11 Bible tool

    this hope we have as an anchor of the soul
    a hope both sure and steadfast
    and one which enters within the veil
    (Hebrews 6:19)

    Anchorman - in the year of our Lord 2021
 *******************************************/

#include <QApplication>
#include "setup.h"
#include "mainwindow.h"
#include "globals.h"
#include "databasehandler.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationVersion("1.01");

    QCommandLineParser parser;
    parser.setApplicationDescription("SoulAnchor - X11 Bible tool");
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
