
TEMPLATE = lib
CONFIG += plugin \
          warn_on \
          link_pkgconfig

TARGET = ffmpeg_decoder

#Enable c++11
CONFIG += c++11

#TODO 暂时链接库文件
#include(../../../Core/Core.pri)
include (../../../Core/CoreHeaders.pri)
include(ffmpeg.pri)

QMAKE_LIBDIR += ../lib

INCLUDEPATH += \
        $$PWD

PKGCONFIG += libavcodec libavformat libavutil

DEFINES += __STDC_CONSTANT_MACROS

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../Core/release/ -lPhoenixPlayer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../Core/debug/ -lPhoenixPlayer
else:unix: LIBS += -L$$OUT_PWD/../../../Core/ -lPhoenixPlayer
