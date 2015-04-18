TEMPLATE = lib
TARGET = qmlplugins
QT += network quick

#Enable c++11
CONFIG += c++11

#TODO 暂时链接库文件
#include(../../../Core/Core.pri)
include (../../Core/CoreHeaders.pri)

include (qml.pri)


INCLUDEPATH += \
        $$PWD


