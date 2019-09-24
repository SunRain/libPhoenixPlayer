
CONFIG += link_pkgconfig

PKGCONFIG += libavcodec libavformat libavutil

DEFINES += __STDC_CONSTANT_MACROS

HEADERS += \
    $$PWD/FFmpeg.h

SOURCES += \
    $$PWD/FFmpeg.cpp


INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

