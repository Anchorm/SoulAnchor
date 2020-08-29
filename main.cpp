 /******************************************************
 * SoulAnchor - X11 Bible tool
 * for daily reading of the Word and some light study
 *
 * this hope we have as an anchor of the soul
 * a hope both sure and steadfast
 * and one which enters within the veil
 * Hebrews 6:19
 *
 * Anchorman - in the year of our Lord 2020
 * ***********************************************/

#include <QApplication>
#include "setup.h"
#include "mainwindow.h"
#include "globals.h"
#include "databasehandler.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationVersion("0.9.2");

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

    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    if (settings.value("showIntro", "true").toString() == "true")
        mainW.showIntro(); 

    return app.exec();
}
