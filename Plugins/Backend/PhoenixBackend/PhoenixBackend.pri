
include(Decoder/default/ffmpeg.pri)
include(OutPut/default/QtAudio.pri)

INCLUDEPATH += $$PWD \
    $$PWD/OutPut \
    $$PWD/Decoder

DEPENDPATH += $$PWD \
    $$PWD/OutPut \
    $$PWD/Decoder

HEADERS += \
    $$PWD/AudioConverter.h \
    $$PWD/AudioEffect.h \
    $$PWD/AudioParameters.h \
    $$PWD/Buffer.h \
    $$PWD/ChannelConverter.h \
    $$PWD/ChannelMap.h \
    $$PWD/DecodeThread.h \
    $$PWD/Decoder/IDecoder.h \
    $$PWD/Dithering.h \
    $$PWD/InternalEvent.h \
    $$PWD/OutPut/IOutPut.h \
    $$PWD/OutputThread.h \
    $$PWD/PhoenixBackend_global.h \
    $$PWD/PhoenixPlayBackend.h \
    $$PWD/Recycler.h \
    $$PWD/SoftVolume.h \
    $$PWD/StateHandler.h \
#    $$PWD/audioconverter.h \
#    $$PWD/channelconverter_p.h \
#    $$PWD/dithering_p.h \
    $$PWD/equ/iir.h \
    $$PWD/equ/iir_cfs.h \
    $$PWD/equ/iir_fpu.h \
    $$PWD/replaygain_p.h

SOURCES += \
    $$PWD/AudioConverter.cpp \
    $$PWD/AudioEffect.cpp \
    $$PWD/AudioParameters.cpp \
    $$PWD/ChannelConverter.cpp \
    $$PWD/ChannelMap.cpp \
    $$PWD/DecodeThread.cpp \
    $$PWD/Decoder/IDecoder.cpp \
    $$PWD/Dithering.cpp \
    $$PWD/InternalEvent.cpp \
    $$PWD/OutPut/IOutPut.cpp \
    $$PWD/OutputThread.cpp \
    $$PWD/PhoenixPlayBackend.cpp \
    $$PWD/Recycler.cpp \
    $$PWD/SoftVolume.cpp \
    $$PWD/StateHandler.cpp \
#    $$PWD/audioconverter.cpp \
#    $$PWD/channelconverter.cpp \
#    $$PWD/dithering.cpp \
    $$PWD/equ/iir.c \
    $$PWD/equ/iir_cfs.c \
    $$PWD/equ/iir_fpu.c \
    $$PWD/replaygain.cpp

OTHER_FILES += \
    $$PWD/PhoenixBackend.json
