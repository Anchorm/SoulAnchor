/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

#include "setup.h"
#include "globals.h"

void setDataDir()
{
    // setup system data directory
    // soulanchor.pro directive
    dataDir.setPath(QString(APP_DATADIR_PREFIX) + "/share/soulanchor");

    if (!dataDir.exists()) {
        ::sout << "System data directory not found! It should be here:" << Qt::endl;
        ::sout << ::dataDir.path() << Qt::endl;
        exit(1);
    } else {
        ::sout << "System data directory: " << ::dataDir.path() << Qt::endl;
    }
}

void setUserDataDir()
{
    // setup user data directory
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
        ::sout << "User data directory: " << userDataDir.path() << Qt::endl;
    }

    ::userDataDir.mkdir("db");

    // silently create or ignore if they fail since these are optional, see MOD file
    ::userDataDir.mkdir("notes");
    ::userDataDir.mkdir("audio-bible");
    ::userDataDir.mkdir("music");
    ::userDataDir.mkdir("music/hymns");
    ::userDataDir.mkdir("music/psalms");
}

void setUserCfgDirAndSettings()
{
    // Qt documentation: QStandardPaths::AppConfigLocation	18	Returns a directory location where user-specific configuration files should be written. This is an application-specific directory, and the returned path is never empty. This enum value was added in Qt 5.5.

    QString cfgPath = QDir(QStandardPaths::writableLocation(
                                  QStandardPaths::AppConfigLocation)).absolutePath();
    ::userCfgDir.setPath(cfgPath);

    if (!::userCfgDir.exists()) {

        if (::userCfgDir.mkpath(cfgPath)) {
            ::sout << ::userCfgDir.path() << " created" << Qt::endl;
        } else {
            ::sout << "User config path not found and could not be created." << Qt::endl;
            ::sout << " check: " << cfgPath << Qt::endl;
            exit(1);
        }
    } else {
        ::sout << "User config directory: " << ::userCfgDir.path() << Qt::endl;
    }

    QFile configFile(::userCfgDir.path() + "/soulanchor.conf");

    if (!configFile.exists()) {

        if (QFile::copy(":/data/skel/soulanchor.conf", ::userCfgDir.path() + "/soulanchor.conf")){
            configFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner
                                      | QFileDevice::ReadGroup | QFileDevice::ReadOther);
            ::sout << " soulanchor.conf created in " << ::userCfgDir.path() << Qt::endl;
        } else {
            ::sout << " could not create soulanchor.conf in " << ::userCfgDir.path() << Qt::endl;
            exit(1);
        }
    }

    ::settingsFile.setFileName(configFile.fileName());
    // create a settings object whenever you need it

    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    QString version = settings.value("version").toString();
    if (version != QGuiApplication::applicationVersion()) {
        QString msg = "soulanchor.conf is not the correct version\n";
        ::sout << "soulanchor.conf is not the correct version" << Qt::endl;

        if (settingsFile.remove()) {
            msg.append("soulanchor.conf removed, restart needed\n");
            ::sout << "soulanchor.conf removed, restart needed" << Qt::endl;
        } else {
            msg.append("could not delete soulanchor.conf, manual deletion needed\n");
            msg.append("check: " + ::userCfgDir.path());
            ::sout << "could not delete soulanchor.conf, manual deletion needed" << Qt::endl;
            ::sout << "check: " << ::userCfgDir.path();
        }

        QMessageBox msgBox;
        msgBox.setText(msg);
        msgBox.exec();
        exit(1);
    }

// Qt documentation:   Sometimes you do want to access settings stored in a specific file or registry path. On all platforms, if you want to read an INI file directly, you can use the QSettings constructor that takes a file name as first argument and pass QSettings::IniFormat as second argument. For example:
}

void LanguageManager::changeLanguage(const QString& langCode) {
    QString lang = langCode.toLower();
    if (m_translator.load("soulanchor_" + lang, dataDir.path() + "/locales")) {
        ::sout << " Language file loaded: " << "soulanchor_" << lang << ".qm";
        qApp->removeTranslator(&m_translator);

        if (qApp->installTranslator(&m_translator))
            ::sout << " Translator installed " << Qt::endl;

    } else {
        ::sout << " could not load this gui translation file: " << "soulanchor_"
               << lang << ".qm" << " " << Qt::endl;
    }
};

QTranslator LanguageManager::m_translator;

