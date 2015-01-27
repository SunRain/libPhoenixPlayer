#-------------------------------------------------
#
# Project created by QtCreator 2015-01-08T19:43:01
#
#-------------------------------------------------

#QT       += core

#QT       -= gui

#TARGET = PhoenixPlayer
#CONFIG   += console
#CONFIG   -= app_bundle

TEMPLATE = subdirs
CONFIG += ordered
#QMAKE_CXXFLAGS += -std=c++0x
#CONFIG += c++11

common.file = Common/Common.pro
SUBDIRS += common
#Plugins.depends = common

musiclibrarycore.file = MusicLibrary/Core/MusicLibraryCore.pro
musiclibrarycore.depends = common
SUBDIRS += musiclibrarycore

SUBDIRS += \
    Backend \
    MusicLibrary \
#    Player \
#    Common \
    test

#gstreamer {
#    gstreamer.file = backend/GStreamer/GStreamer.pro
#    SUBDIRS += gstreamer
#}

#test {
#    test.file = test/test.pro
#    SUBDIRS += test
##    test.depends = backend/GStreamer
#}


