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

include(../../Common/Common.pri)
include(../../MusicLibrary/Core/MusicLibraryCore.pri)
include(../../Player/Player.pri)
include (../../MetadataLookup/Core/MetadataLookupCore.pri)
include (../../Plugins/QML/qml.pri)

