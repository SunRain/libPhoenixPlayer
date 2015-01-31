
INCLUDEPATH += $$PWD/include

HEADERS += \
    $$PWD/include/Backend/BaseMediaObject.h \
    $$PWD/include/Backend/IPlayBackend.h \
    $$PWD/include/Common.h \
    $$PWD/include/Settings.h \
    $$PWD/include/SongMetaData.h \
    $$PWD/include/Util.h \
    $$PWD/include/MusicLibrary/IPlayListDAO.h \
    $$PWD/include/MusicLibrary/IMusicTagParser.h \
    $$PWD/include/PluginLoader.h \
    $$PWD/include/Lyrics/ILyricsLookup.h

SOURCES += \
    $$PWD/BaseMediaObject.cpp \
    $$PWD/Settings.cpp \
    $$PWD/SongMetaData.cpp \
    $$PWD/Util.cpp \
    $$PWD/IPlayListDAO.cpp \
    $$PWD/IMusicTagParser.cpp \
    $$PWD/PluginLoader.cpp




