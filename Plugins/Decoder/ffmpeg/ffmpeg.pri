
CONFIG += link_pkgconfig

PKGCONFIG += libavcodec libavformat libavutil

DEFINES += __STDC_CONSTANT_MACROS

OTHER_FILES += \
    $$PWD/ffmpeg_decoder.json

HEADERS += \
    $$PWD/FFmpeg.h

SOURCES += \
    $$PWD/FFmpeg.cpp




