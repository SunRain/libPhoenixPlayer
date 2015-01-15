

INCLUDEPATH += $$PWD/include

HEADERS += \
#    $$PWD/include/IPlayList.h
    $$PWD/include/DiskLookup.h \
    $$PWD/include/IPlayListDAO.h \
    $$PWD/include/PlayListDAOLoader.h \
    $$PWD/include/MusicLibraryManager.h

SOURCES += \
    $$PWD/DiskLookup.cpp \
    $$PWD/IPlayListDAO.cpp \
    $$PWD/PlayListDAOLoader.cpp \
    $$PWD/MusicLibraryManager.cpp


