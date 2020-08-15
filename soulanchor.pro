QT += core gui widgets sql multimedia

TARGET = soulanchor
DESTDIR = install/usr/bin

# documentation.path = install/usr/share/doc/soulanchor
# documentation.files = README.md LICENSE MOD COPYING
# INSTALLS += documentation

TEMPLATE = app
TRANSLATIONS = languages/soulanchor_en.ts  languages/soulanchor_nl.ts

DEFINES += \ 
        QT_DEPRECATED_WARNINGS \
        D_GLIBCXX_ASSERTIONS \
        D_FORTIFY_SOURCE=2

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

