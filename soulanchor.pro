QT += core gui widgets sql multimedia

TARGET = soulanchor
DESTDIR = install/usr/bin/

# usage from cmd line: qmake PREFIX=/opt

isEmpty(PREFIX) {
 PREFIX = /usr
}

binary.files = install/usr/bin/soulanchor
binary.path = $$PREFIX/bin/

documentation.files = README.md MOD
documentation.path = $$PREFIX/share/doc/soulanchor/

license.files = LICENSE
license.path = $$PREFIX/share/licenses/soulanchor/

databases.files = install/usr/share/soulanchor/db/*.db
databases.path = $$PREFIX/share/soulanchor/db/

images.files = install/usr/share/soulanchor/img/*.jpg
images.files += install/usr/share/soulanchor/img/*.png
images.path = $$PREFIX/share/soulanchor/img/

icon.files = install/usr/share/pixmaps/soulanchor.png
icon.path = $$PREFIX/share/pixmaps/

desktop.files = install/usr/share/applications/soulanchor.desktop
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
TRANSLATIONS = languages/soulanchor_en.ts  languages/soulanchor_nl.ts

DEFINES += D_GLIBCXX_ASSERTIONS D_FORTIFY_SOURCE=2

QMAKE_CXXFLAGS += -fstack-clash-protection
QMAKE_LFLAGS += -Wl,-z,relro,-z,now

CONFIG += release c++17
        
RESOURCES += soulanchor.qrc
        
SOURCES += \
        aboutwindow.cpp \
        databasehandler.cpp \
        exportbible.cpp \
        globals.cpp \
        main.cpp \
        mainwindow.cpp \
        makehtmlbible.cpp \
        parwindow.cpp \
        roster.cpp \
        settingswindow.cpp \
        setup.cpp

HEADERS += \
        aboutwindow.h \
        databasehandler.h \
        exportbible.h \
        globals.h \
        mainwindow.h \
        makehtmlbible.h \
        parwindow.h \
        roster.h \
        settingswindow.h \
        setup.h

FORMS += \
        exportbible.ui \
        mainwindow.ui \
        makehtmlbible.ui \
        roster.ui \
        settingswindow.ui

