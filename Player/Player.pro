
TEMPLATE = lib
CONFIG += plugin

QT       += core

TARGET = PhoenixPlayer
QMAKE_CXXFLAGS += -std=c++0x

include (Player.pri)
include (../Backend/Core/BackendCore.pri)
include (../Common/Common.pri)
include (../MusicLibrary/Core/MusicLibraryCore.pri)

unix {
#    CONFIG += link_pkgconfig
#    PKGCONFIG += gstreamer-0.10

#    INCLUDEPATH += \

}

DESTDIR = ../plugins
