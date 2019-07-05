OTHER_FILES += \
    $$PWD/SpectrumGeneratorFFmpeg.json

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

include($$PWD/qt-spek.pri)

HEADERS += \
    $$PWD/SpectrumGeneratorFFmpeg.h

SOURCES += \
    $$PWD/SpectrumGeneratorFFmpeg.cpp
