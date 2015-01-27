
INCLUDEPATH += $$PWD/include

HEADERS += \
    $$PWD/include/backend/BaseMediaObject.h \
    $$PWD/include/backend/IPlayBackend.h \
    $$PWD/include/Common.h \
    $$PWD/include/Settings.h \
    $$PWD/include/SongMetaData.h \
    $$PWD/include/Util.h \
    $$PWD/include/MusicLibrary/IPlayListDAO.h \
    $$PWD/include/MusicLibrary/IMusicTagParser.h

SOURCES += \
    $$PWD/BaseMediaObject.cpp \
    $$PWD/Settings.cpp \
    $$PWD/SongMetaData.cpp \
    $$PWD/Util.cpp \
    $$PWD/IPlayListDAO.cpp \
    $$PWD/IMusicTagParser.cpp




