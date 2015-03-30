TEMPLATE = lib
TARGET = qmlplugins
QT += network quick

#Enable c++11
CONFIG += c++11

include (../../Common/sdk.pri)
include (../../MetadataLookup/Core/MetadataLookupCore.pri)
include (../../MusicLibrary/Core/MusicLibraryCore.pri)
include (../../Player/PlayerSDK.pri)
include (qml.pri)


INCLUDEPATH += \
        $$PWD


