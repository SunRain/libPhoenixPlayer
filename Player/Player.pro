
TEMPLATE = lib
CONFIG += plugin

TARGET = PhoenixPlayer

include (Player.pri)
include (../Backend/Core/BackendCore.pri)
include (../Common/Common.pri)
include (../MusicLibrary/Core/MusicLibrary.pri)

unix {
#    CONFIG += link_pkgconfig
#    PKGCONFIG += gstreamer-0.10

#    INCLUDEPATH += \

}

DESTDIR = ../../../plugins
