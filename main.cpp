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
#include <QStyleFactory>

int main(int argc, char *argv[]) {
    QApplication::setApplicationVersion("1.09");
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("SoulAnchor - X11 Bible reading tool");
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    setDataDir();
    setUserDataDir();
    setUserCfgDirAndSettings();

    MainWindow mainW;
    mainW.show();

    return app.exec();
}
