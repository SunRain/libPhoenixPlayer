
INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

QT += qml
#Enable c++11
CONFIG += c++11

HEADERS += \
    $$PWD/include/Common.h \
    $$PWD/include/PluginLoader.h \
    $$PWD/include/Settings.h \
    $$PWD/include/SingletonPointer_p.h \
    $$PWD/include/SingletonPointer_sf.h \
    $$PWD/include/SingletonPointer.h \
    $$PWD/include/SongMetaData.h \
    $$PWD/include/Util.h \
    $$PWD/include/Backend/BaseMediaObject.h \
    $$PWD/include/Backend/IPlayBackend.h \
    $$PWD/include/MetadataLookup/BaseNetworkLookup.h \
    $$PWD/include/MetadataLookup/IMetadataLookup.h \
    $$PWD/include/MetadataLookup/LyricsParser.h \
    $$PWD/include/MusicLibrary/IMusicTagParser.h \
    $$PWD/include/MusicLibrary/IPlayListDAO.h \
    $$PWD/include/MusicLibrary/MusicLibraryManager.h \
    $$PWD/include/Player/Player.h \
    $$PWD/MusicLibrary/DiskLookup.h \
    $$PWD/MusicLibrary/TagParserManager.h \
    $$PWD/include/PluginHost.h \
    $$PWD/include/AddonMgr.h \
    $$PWD/MusicLibrary/AsyncDiskLookup.h \
    $$PWD/MusicLibrary/AsyncTagParserMgrWrapper.h \
    $$PWD/include/MusicLibrary/LocalMusicSacnner.h \
    $$PWD/include/MetadataLookup/MetadataLookupMgr.h \
    $$PWD/include/MetadataLookup/MetadataLookupMgrWrapper.h \
    $$PWD/include/Decoder/IDecoder.h \
    $$PWD/include/Buffer.h \
    $$PWD/include/AudioParameters.h \
    $$PWD/include/OutPut/IOutPut.h \
    $$PWD/include/Backend/BaseVolume.h \
    $$PWD/include/Player/VolumeControl.h \
    $$PWD/include/Backend/SoftVolume.h \
    $$PWD/include/BaseObject.h \
    $$PWD/include/Backend/BaseVisual.h \
    $$PWD/include/EqualizerMgr.h

SOURCES += \
    $$PWD/MetadataLookup/BaseNetworkLookup.cpp \
    $$PWD/MetadataLookup/IMetadataLookup.cpp \
    $$PWD/MetadataLookup/LyricsParser.cpp \
    $$PWD/MusicLibrary/DiskLookup.cpp \
    $$PWD/MusicLibrary/MusicLibraryManager.cpp \
    $$PWD/MusicLibrary/TagParserManager.cpp \
    $$PWD/Player/Player.cpp \
    $$PWD/BaseMediaObject.cpp \
    $$PWD/IMusicTagParser.cpp \
    $$PWD/IPlayListDAO.cpp \
    $$PWD/PluginLoader.cpp \
    $$PWD/Settings.cpp \
    $$PWD/SongMetaData.cpp \
    $$PWD/Util.cpp \
    $$PWD/PluginHost.cpp \
    $$PWD/AddonMgr.cpp \
    $$PWD/MusicLibrary/AsyncDiskLookup.cpp \
    $$PWD/MusicLibrary/AsyncTagParserMgrWrapper.cpp \
    $$PWD/MusicLibrary/LocalMusicSacnner.cpp \
    $$PWD/MetadataLookup/MetadataLookupMgr.cpp \
    $$PWD/MetadataLookup/MetadataLookupMgrWrapper.cpp \
    $$PWD/Decoder/IDecoder.cpp \
    $$PWD/AudioParameters.cpp \
    $$PWD/OutPut/IOutPut.cpp \
    $$PWD/include/Backend/BaseVolume.cpp \
    $$PWD/Player/VolumeControl.cpp \
    $$PWD/include/Backend/SoftVolume.cpp \
    $$PWD/EqualizerMgr.cpp










