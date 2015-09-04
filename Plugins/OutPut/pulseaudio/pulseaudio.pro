
TEMPLATE = lib
CONFIG += \
        plugin \
        thread \
        warn_on \
        link_pkgconfig

TARGET = pulseaudio

#Enable c++11
CONFIG += c++11

#TODO 暂时链接库文件
#include(../../../Core/Core.pri)
include (../../../Core/CoreHeaders.pri)
include(pulseaudio.pri)

QMAKE_LIBDIR += ../lib

PKGCONFIG += libpulse-simple

INCLUDEPATH += \
        $$PWD

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../Core/release/ -lPhoenixPlayer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../Core/debug/ -lPhoenixPlayer
else:unix: LIBS += -L$$OUT_PWD/../../../Core/ -lPhoenixPlayer
