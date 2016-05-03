
OTHER_FILES += \
    $$PWD/PhoenixBackend.json

SOURCES += \
    $$PWD/StateHandler.cpp \
    $$PWD/PlayThread.cpp \
    $$PWD/StateChangedEvent.cpp \
    $$PWD/PhoenixPlayBackend.cpp \
    $$PWD/equ/iir_cfs.c \
    $$PWD/equ/iir_fpu.c \
    $$PWD/equ/iir.c \
    $$PWD/OutputThread.cpp

HEADERS += \
    $$PWD/PhoenixBackend_global.h \
    $$PWD/StateHandler.h \
    $$PWD/PlayThread.h \
    $$PWD/StateChangedEvent.h \
    $$PWD/PhoenixPlayBackend.h \
    $$PWD/equ/iir_cfs.h \
    $$PWD/equ/iir_fpu.h \
    $$PWD/equ/iir.h \
    $$PWD/RingBuffer.h \
    $$PWD/OutputThread.h

DISTFILES += \
    $$PWD/OutputThread-bak.h-bak \
    $$PWD/OutputThread-bak.cpp-bak \
    $$PWD/Recycler.cpp-bak \
    $$PWD/Recycler.h \
