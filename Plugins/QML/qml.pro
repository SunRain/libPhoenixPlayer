TEMPLATE = lib
TARGET = qmlplugins
QT += network

#Enable c++11
CONFIG += c++11

include (../../Common/sdk.pri)
include (../../Lyrics/Core/LyricsCore.pri)
include (qml.pri)


INCLUDEPATH += \
        $$PWD


