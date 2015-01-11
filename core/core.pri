

_SRC = $$PWD

INCLUDEPATH += $$_SRC/include

HEADERS += \
    $$_SRC/include/BasePlayer.h \
#    $$_SRC/include/IPlayList.h \
    $$_SRC/include/IPlayBackend.h \
    $$_SRC/include/Common.h \
    $$_SRC/include/SongMetaDate.h \
    $$_SRC/include/PlayBackendLoader.h \
    $$_SRC/include/BaseMediaObject.h


SOURCES += \
    $$_SRC/BasePlayer.cpp \
#    $$_SRC/BasePlayList.cpp \
    $$_SRC/MetaData.cpp \
    $$_SRC/PlayBackendLoader.cpp \
    $$PWD/BaseMediaObject.cpp

