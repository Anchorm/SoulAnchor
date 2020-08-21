QT += core gui widgets sql multimedia

TARGET = soulanchor
DESTDIR = install/bin/

# usage from cmd line: qmake PREFIX=/opt

isEmpty(PREFIX) {
 PREFIX = /usr
}

binary.files = install/bin/soulanchor
binary.path = $$PREFIX/bin/

documentation.files = README.md MOD
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

INSTALLS += \
        binary \
        documentation \
        license \
        databases \
        images \
        icon \
        desktop

TEMPLATE = app

TRANSLATIONS = \
        data/lang/soulanchor_en.ts \
        data/lang/soulanchor_nl.ts

# super easy syntax
DEFINES += APP_DATADIR_PREFIX='\\"$$PREFIX\\"'

# QMAKE_CXXFLAGS += -fstack-clash-protection
# QMAKE_LFLAGS += -Wl,-z,relro,-z,now

CONFIG += release c++17

RESOURCES += soulanchor.qrc

SOURCES += \
        aboutwindow.cpp \
        databasehandler.cpp \
        globals.cpp \
        main.cpp \
        mainwindow.cpp \
        parwindow.cpp \
        roster.cpp \
        settingswindow.cpp \
        setup.cpp

HEADERS += \
        aboutwindow.h \
        databasehandler.h \
        globals.h \
        mainwindow.h \
        parwindow.h \
        roster.h \
        settingswindow.h \
        setup.h

FORMS += \
        mainwindow.ui \
        roster.ui \
        settingswindow.ui
