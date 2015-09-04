TEMPLATE = lib
CONFIG += plugin

QT += network

TARGET = LyricsBaidu

#Enable c++11
CONFIG += c++11

#TODO 暂时链接库文件
#include(../../../Core/Core.pri)
include (../../../Core/CoreHeaders.pri)

include (Baidu.pri)

INCLUDEPATH += \
        $$PWD

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += taglib
}

#DESTDIR = ../../../plugins

QMAKE_LIBDIR += ../lib

win32 {
        INCLUDEPATH += $(TAGLIB_DIR)/include
        LIBS += -L$(TAGLIB_DIR)/lib -ltag
        DEFINES += TAGLIB_STATIC
}


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../Core/release/ -lPhoenixPlayer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../Core/debug/ -lPhoenixPlayer
else:unix: LIBS += -L$$OUT_PWD/../../../Core/ -lPhoenixPlayer

