
TEMPLATE = lib
#CONFIG += plugin

QT       += core network

TARGET = PhoenixPlayer
#QMAKE_CXXFLAGS += -std=c++0x
#Enable c++11
CONFIG += c++11

include (Player.pri)
include (../Common/sdk.pri)
include (../MusicLibrary/Core/MusicLibrarySDK.pri)
include (../Lyrics/Core/LyricsCore.pri)

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Common \
#                                              -L$$OUT_PWD/../MusicLibraryCore \
#                                              -lCommon \
#                                              -lMusicLibraryCore
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Common \
#                                                 -L$$OUT_PWD/../MusicLibraryCore \
#                                                 -lCommon \
#                                                 -lMusicLibraryCore
#else:unix: LIBS += -L$$OUT_PWD/../Common \
#                   -L$$OUT_PWD/../MusicLibraryCore \
#                   -lCommon \
#                   -lMusicLibraryCore

unix {
#    CONFIG += link_pkgconfig
#    PKGCONFIG += gstreamer-0.10

#    INCLUDEPATH += \

}

DESTDIR = ../plugins

win32:CONFIG(release, debug|release): LIBS += \
        -L$$OUT_PWD/../Common/release/ \
        -L$$OUT_PWD/../MusicLibrary/Core/release/ \
        -lCommon \
        -lMusicLibraryCore
else:win32:CONFIG(debug, debug|release): LIBS += \
        -L$$OUT_PWD/../Common/debug/ \
        -L$$OUT_PWD/../MusicLibrary/Core/debug/\
        -lCommon \
        -lMusicLibraryCore
else:unix: LIBS += \
        -L$$OUT_PWD/../Common/ \
        -L$$OUT_PWD/../MusicLibrary/Core/ \
        -lCommon \
        -lMusicLibraryCore



