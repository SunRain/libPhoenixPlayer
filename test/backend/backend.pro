#-------------------------------------------------
#
# Project created by QtCreator 2015-01-08T19:43:01
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = testbackend
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp

unix {
    INCLUDEPATH += \
#        $$PWD/../core/include \
#        $$PWD/../backend/GStreamerBackend \
        /usr/include/gstreamer-0.10 \
        /usr/include/glib-2.0 \
        /usr/include/libxml2
}

#unix: LIBS += -L$$OUT_PWD/../PlayBackend -lplugin_gstreamer

#SOURCES += \
#    main.cpp \

#include (../../Core/Core.pri)
include(../../Core/CoreHeaders.pri)

#QMAKE_LIBDIR += lib
QMAKE_RPATHDIR += $$OUT_PWD/lib

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Core/release/ -lPhoenixPlayer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Core/debug/ -lPhoenixPlayer
else:unix: LIBS += -L$$OUT_PWD/../../Core/ -lPhoenixPlayer

INCLUDEPATH += $$PWD/../../Core
DEPENDPATH += $$PWD/../../Core
