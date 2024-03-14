QT += core gui widgets sql multimedia

TARGET = soulanchor
DESTDIR = install/bin/

# usage from cmd line: qmake6 PREFIX=/opt
isEmpty(PREFIX) {
 PREFIX = /usr
}

binary.files = install/bin/soulanchor
binary.path = $$PREFIX/bin/

documentation.files = README.md
documentation.files += install/share/doc/soulanchor/mod.html
documentation.path = $$PREFIX/share/doc/soulanchor/

license.files = LICENSE
license.path = $$PREFIX/share/licenses/soulanchor/

databases.files = install/share/soulanchor/db/*.db
databases.path = $$PREFIX/share/soulanchor/db/

images.files = install/share/soulanchor/img/*.jpg
images.files += install/share/soulanchor/img/*.png
images.path = $$PREFIX/share/soulanchor/img/

icon.files = install/share/pixmaps/soulanchor.png
icon.path = $$PREFIX/share/pixmaps/

desktop.files = install/share/applications/soulanchor.desktop
desktop.path = $$PREFIX/share/applications/

locales.files = install/share/soulanchor/locales/*.qm
locales.path = $$PREFIX/share/soulanchor/locales

INSTALLS += \
        binary \
        documentation \
        license \
        databases \
        images \
        icon \
        desktop \
        locales

TEMPLATE = app

TRANSLATIONS += \
        data/locales/soulanchor_en.ts \
        data/locales/soulanchor_nl.ts

DEFINES += APP_DATADIR_PREFIX='\\"$$PREFIX\\"'

CONFIG += release c++17

RESOURCES += soulanchor.qrc

SOURCES += \
        aboutwindow.cpp \
        clickablelabel.cpp \
        databasehandler.cpp \
        flowlayout.cpp \
        globals.cpp \
        imagewindow.cpp \
        importwindow.cpp \
        main.cpp \
        mainwindow.cpp \
        parwindow.cpp \
        roster.cpp \
        settingswindow.cpp \
        setup.cpp

HEADERS += \
        aboutwindow.h \
        clickablelabel.h \
        databasehandler.h \
        flowlayout.h \
        globals.h \
        imagewindow.h \
        importwindow.h \
        mainwindow.h \
        parwindow.h \
        roster.h \
        settingswindow.h \
        setup.h \
        utilities.h

FORMS += \
        mainwindow.ui \
        roster.ui \
        settingswindow.ui
