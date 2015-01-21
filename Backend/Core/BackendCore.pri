

INCLUDEPATH += $$PWD/include

HEADERS += \
    $$PWD/include/BasePlayer.h \
    $$PWD/include/IPlayBackend.h \
    $$PWD/include/PlayBackendLoader.h \
    $$PWD/include/BaseMediaObject.h


SOURCES += \
    $$PWD/BasePlayer.cpp \
#    $$PWD/MetaData.cpp \
    $$PWD/PlayBackendLoader.cpp \
    $$PWD/BaseMediaObject.cpp

