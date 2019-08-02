#-------------------------------------------------
#
# Project created by QtCreator 2019-08-02T23:14:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PhoenixBackend_demo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include("../../../../../libPhoenixPlayer/Core/CoreHeaders.pri")
#include("../../../../../ThirdParty/QCurl/src/qcurl.pri")

CONFIG += c++11
CONFIG -= WITH_QML_LIB
DEFINES -= WITH_QML_LIB
QMAKE_CXXFLAGS = -fpermissive

INCLUDEPATH += $$PWD/../..
DEPENDPATH += $$PWD/../..

SOURCES += \
        ../../AudioConverter.cpp \
        ../../AudioEffect.cpp \
        ../../BufferQueue.cpp \
        ../../ChannelConverter.cpp \
        ../../DecodeThread.cpp \
        ../../InternalEvent.cpp \
        ../../OutputThread.cpp \
        ../../PhoenixPlayBackend.cpp \
        ../../PlayThread.cpp \
        ../../StateHandler.cpp \
        ../../equ/iir.c \
        ../../equ/iir_cfs.c \
        ../../equ/iir_fpu.c \
        main.cpp \
        MainWindow.cpp

HEADERS += \
        ../../AudioConverter.h \
        ../../AudioEffect.h \
        ../../BufferQueue.h \
        ../../ChannelConverter.h \
        ../../DecodeThread.h \
        ../../InternalEvent.h \
        ../../OutputThread.h \
        ../../OutputThread_old.h \
        ../../PhoenixBackend_global.h \
        ../../PhoenixPlayBackend.h \
        ../../PhoenixPlayer.h \
        ../../PlayThread.h \
        ../../RingBuffer.h \
        ../../StateHandler.h \
        ../../equ/iir.h \
        ../../equ/iir_cfs.h \
        ../../equ/iir_fpu.h \
        MainWindow.h

isEmpty (LIB_DIR){
    LIB_DIR = /opt/PhoenixPlayer
}

QMAKE_LIBDIR += \
    $${OUT_PWD} \
    $${LIB_DIR}/lib \

QMAKE_RPATHDIR += \
    $${OUT_PWD} \
    $${LIB_DIR}/lib \

LIBS += -lPhoenixPlayer

DISTFILES += \
    ../../PhoenixBackend.json \
    ../../PhoenixBackend.pri \
    ../../README.md

SUBDIRS += \
    ../../PhoenixBackend.pro
