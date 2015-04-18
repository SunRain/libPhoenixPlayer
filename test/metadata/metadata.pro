#-------------------------------------------------
#
# Project created by QtCreator 2015-01-08T19:43:01
#
#-------------------------------------------------

QT       += core network quick

TARGET = testmetadata


TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp

include(../../Core/CoreHeaders.pri)
#include (../../Plugins/QML/QmlPluginHeaders.pri)


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Core/release/ -lPhoenixPlayer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Core/debug/ -lPhoenixPlayer
else:unix: LIBS += -L$$OUT_PWD/../../Core/ -lPhoenixPlayer
