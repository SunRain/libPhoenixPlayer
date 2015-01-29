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

include(../../Common/Common.pri)
