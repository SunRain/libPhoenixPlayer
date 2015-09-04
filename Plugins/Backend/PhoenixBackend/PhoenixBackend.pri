


OTHER_FILES += \
    $$PWD/PhoenixBackend.json

SOURCES += \
    $$PWD/Recycler.cpp \
    $$PWD/OutputThread.cpp \
    $$PWD/StateHandler.cpp \
    $$PWD/PlayThread.cpp \
    $$PWD/StateChangedEvent.cpp \
    $$PWD/PhoenixPlayBackend.cpp \
    $$PWD/equ/iir_cfs.c \
    $$PWD/equ/iir_fpu.c \
    $$PWD/equ/iir.c

HEADERS += \
    $$PWD/Recycler.h \
    $$PWD/PhoenixBackend_global.h \
    $$PWD/OutputThread.h \
    $$PWD/StateHandler.h \
    $$PWD/PlayThread.h \
    $$PWD/StateChangedEvent.h \
    $$PWD/PhoenixPlayBackend.h \
    $$PWD/equ/iir_cfs.h \
    $$PWD/equ/iir_fpu.h \
    $$PWD/equ/iir.h
