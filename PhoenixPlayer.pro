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

SUBDIRS += \
    Backend \
    PlayList \
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


