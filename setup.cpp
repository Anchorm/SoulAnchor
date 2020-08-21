#include "setup.h"

void setDataDir()
{
    bool dirExists = false;
    bool biblesExist, dictsExist, devExist = false;
    // soulanchor.pro defines
    dataDir.setPath(QString(APP_DATADIR_PREFIX) + "/share/soulanchor");

    if (dataDir.exists()) {
        biblesExist = QFile(dataDir.path() + "/db/bibles.db").exists();
        dictsExist = QFile(dataDir.path() + "/db/dictionaries.db").exists();
        devExist = QFile(dataDir.path() + "/db/devotions.db").exists();

        if (biblesExist && dictsExist && devExist) {
            dirExists = true;
        }
    }

    if (!dirExists) {
        ::sout << "App data directory not found in:" << Qt::endl;
        ::sout << APP_DATADIR_PREFIX << Qt::endl;
        exit(1);
    } else {
        ::sout << "App data dir: " << dataDir.path() << Qt::endl;
    }
}

void setUserDataDir() {
    ::userDataDir.setPath(QDir(QStandardPaths::writableLocation
                               (QStandardPaths::AppDataLocation)).path());

    if (!::userDataDir.exists()) {
        if (::userDataDir.mkpath(".")) {
            ::sout << ::userDataDir.path() << " created" << Qt::endl;

        } else {
            ::sout << "Cannot find and/or create writable location for user data." << Qt::endl;
            ::sout << "E.g. $HOME/.local/share/soulanchor" << Qt::endl;
            exit(1);
        }

    } else {
        ::sout << "User data dir: " << userDataDir.path() << Qt::endl;
    }

    ::userDataDir.mkdir("notes");
    ::userDataDir.mkdir("audio-bible");
    ::userDataDir.mkdir("music");
    ::userDataDir.mkdir("music/hymns");
    ::userDataDir.mkdir("music/psalms");
}

void setUserCfgDirAndSettings() {
    // Qt documentation: QStandardPaths::AppConfigLocation	18	Returns a directory location where user-specific configuration files should be written. This is an application-specific directory, and the returned path is never empty. This enum value was added in Qt 5.5.

    QString cfgPath = QDir(QStandardPaths::writableLocation(
                                  QStandardPaths::AppConfigLocation)).absolutePath();
    ::userCfgDir.setPath(cfgPath);

    if (!::userCfgDir.exists()) {

        if (::userCfgDir.mkpath(cfgPath)) {
            ::sout << ::userCfgDir.path() << " created" << Qt::endl;
        } else {
            ::sout << " App config path not found and could not be created." << Qt::endl;
            ::sout << "path: " << cfgPath << Qt::endl;
            exit(1);
        }
    } else {
        ::sout << "Config dir:" << ::userCfgDir.path() << Qt::endl;
    }

    QFile configFile(::userCfgDir.path() + "/soulanchor.conf");

    if (!configFile.exists()) {

        if (QFile::copy(":/data/skel/soulanchor.conf", ::userCfgDir.path() + "/soulanchor.conf")) {
            configFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner
                                      | QFileDevice::ReadGroup | QFileDevice::ReadOther);
            ::sout << " soulanchor.conf created in " << ::userCfgDir.path() << Qt::endl;
        } else {
            ::sout << " could not create soulanchor.conf in " << ::userCfgDir.path() << Qt::endl;
            exit(1);
        }
    }

    ::settingsFile.setFileName(configFile.fileName()) ;
    // create a setting object whenever you need it

// Qt documentation:   Sometimes you do want to access settings stored in a specific file or registry path. On all platforms, if you want to read an INI file directly, you can use the QSettings constructor that takes a file name as first argument and pass QSettings::IniFormat as second argument. For example:
}

void installTranslator()
{
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    QString lang = settings.value("guiLanguage", "english").toString();
    if ( not(lang == "dutch" || lang == "english") ){
        lang = "english";
    }

    if (lang == "dutch") lang = "nl";
    if (lang == "english") lang = "en";

    if (::translator.load(":/data/lang/soulanchor_" + lang + ".qm") ) {
        qApp->installTranslator(&::translator);
    }
}
