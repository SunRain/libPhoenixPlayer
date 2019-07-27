
OTHER_FILES += \
    $$PWD/PhoenixBackend.json

SOURCES += \
    $$PWD/AudioConverter.cpp \
    $$PWD/AudioEffect.cpp \
    $$PWD/BufferQueue.cpp \
    $$PWD/ChannelConverter.cpp \
    $$PWD/DecodeThread.cpp \
    $$PWD/InternalEvent.cpp \
    $$PWD/OutputThread.cpp \
    $$PWD/OutputThread_old.cpp \
    $$PWD/StateHandler.cpp \
    $$PWD/PlayThread.cpp \
    $$PWD/PhoenixPlayBackend.cpp \
    $$PWD/equ/iir_cfs.c \
    $$PWD/equ/iir_fpu.c \
    $$PWD/equ/iir.c

HEADERS += \
    $$PWD/AudioConverter.h \
    $$PWD/AudioEffect.h \
    $$PWD/BufferQueue.h \
    $$PWD/ChannelConverter.h \
    $$PWD/DecodeThread.h \
    $$PWD/InternalEvent.h \
    $$PWD/OutputThread.h \
    $$PWD/OutputThread_old.h \
    $$PWD/PhoenixBackend_global.h \
    $$PWD/StateHandler.h \
    $$PWD/PlayThread.h \
    $$PWD/PhoenixPlayBackend.h \
    $$PWD/equ/iir_cfs.h \
    $$PWD/equ/iir_fpu.h \
    $$PWD/equ/iir.h \
    $$PWD/RingBuffer.h

DISTFILES += \
    $$PWD/OutputThread-bak.h-bak \
    $$PWD/OutputThread-bak.cpp-bak \
    $$PWD/Recycler.cpp-bak \
    $$PWD/Recycler.h \
