OTHER_FILES += \
    $$PWD/SpectrumGeneratorFFmpeg.json

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/SpectrumGeneratorFFmpeg.h \
    $$PWD/SpekAudio.h \
    $$PWD/SpekFFT.h

SOURCES += \
    $$PWD/SpectrumGeneratorFFmpeg.cpp \
    $$PWD/SpekAudio.cpp \
    $$PWD/SpekFFT.cpp
