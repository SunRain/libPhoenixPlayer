#-------------------------------------------------
#
# Project created by QtCreator 2015-01-08T19:43:01
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = testplaylist
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

unix {
    INCLUDEPATH += \
#        $$PWD/../core/include \
#        $$PWD/../backend/GStreamerBackend \
#        /usr/include/gstreamer-0.10 \
#        /usr/include/glib-2.0 \
#        /usr/include/libxml2
}

#unix: LIBS += -L$$OUT_PWD/../PlayBackend -lplugin_gstreamer

#SOURCES += \
#    main.cpp \

#include(../backend/backend.pri)
#include(../../Core/Core.pri)
include(../../Common/Common.pri)
include(../../PlayList/Core/Core.pri)
