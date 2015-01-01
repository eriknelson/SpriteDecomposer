#-------------------------------------------------
#
# Project created by QtCreator 2011-05-26T15:38:31
#
#-------------------------------------------------

QT       += core gui xml

TARGET = SpriteDecomposer
TEMPLATE = app

SOURCES += main.cpp\
        interface.cpp \
    workarea.cpp \
    animation.cpp \
    cut.cpp \
    frame.cpp \
    sequencer.cpp \
    mark.cpp \
    aboutdialog.cpp \
    application.cpp

ICON = SpriteDecomposer.icns

HEADERS  += interface.h \
    workarea.h \
    animation.h \
    cut.h \
    frame.h \
    sequencer.h \
    mark.h \
    aboutdialog.h \
    application.h

FORMS    += interface.ui \
    aboutdialog.ui

RESOURCES += menu.qrc \
    lang.qrc

TRANSLATIONS = lang/fr_fr.ts \
            lang/en_us.ts
