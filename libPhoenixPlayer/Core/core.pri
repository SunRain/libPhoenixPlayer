
INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

QT += network core
##Enable c++11
#CONFIG += c++11

HEADERS += \
    $$PWD/include/MusicLibrary/ISpectrumGenerator.h \
    $$PWD/include/MusicLibrary/SpectrumGeneratorHost.h \
    $$PWD/include/PPCommon.h \
    $$PWD/include/PPUtility.h \
    $$PWD/include/PlayerCore/PlayListMeta.h \
    $$PWD/include/PlayerCore/PlayListMetaMgr.h \
    $$PWD/include/PlayerCore/PlayListObject.h \
    $$PWD/include/PlayerCore/RecentPlayedQueue.h \
    $$PWD/include/PluginLoader.h \
    $$PWD/include/PPSettings.h \
    $$PWD/include/SingletonPointer_p.h \
    $$PWD/include/SingletonPointer_sf.h \
    $$PWD/include/SingletonPointer.h \
    $$PWD/include/Backend/BaseMediaObject.h \
    $$PWD/include/Backend/IPlayBackend.h \
    $$PWD/include/MetadataLookup/BaseNetworkLookup.h \
    $$PWD/include/MetadataLookup/IMetadataLookup.h \
    $$PWD/include/MetadataLookup/LyricsParser.h \
    $$PWD/include/MusicLibrary/IMusicTagParser.h \
    $$PWD/include/MusicLibrary/IMusicLibraryDAO.h \
    $$PWD/include/MusicLibrary/MusicLibraryManager.h \
    $$PWD/include/PlayerCore/PlayerCore.h \
#    $$PWD/MusicLibrary/DiskLookup.h \
#    $$PWD/MusicLibrary/TagParserManager.h \
    $$PWD/include/PluginHost.h \
    $$PWD/include/AddonMgr.h \
#    $$PWD/MusicLibrary/AsyncDiskLookup.h \
#    $$PWD/MusicLibrary/AsyncTagParserMgrWrapper.h \
    $$PWD/include/MusicLibrary/LocalMusicScanner.h \
    $$PWD/include/MetadataLookup/MetadataLookupMgr.h \
    $$PWD/include/MetadataLookup/MetadataLookupMgrWrapper.h \
#    $$PWD/include/Decoder/IDecoder.h \
#    $$PWD/include/Buffer.h \
#    $$PWD/include/AudioParameters.h \
#    $$PWD/include/OutPut/IOutPut.h \
    $$PWD/include/Backend/BaseVolume.h \
    $$PWD/include/PlayerCore/VolumeControl.h \
#    $$PWD/include/Backend/SoftVolume.h \
    $$PWD/include/BaseObject.h \
    $$PWD/include/Backend/BaseVisual.h \
    $$PWD/include/EqualizerMgr.h \
    $$PWD/PlayerCore/PlayListFormat.h \
    $$PWD/PlayerCore/M3uPlayListFormat.h \
#    $$PWD/include/PlayerCore/PlayQueueMgr.h \
    $$PWD/include/Backend/BackendHost.h \
#    $$PWD/include/Decoder/DecoderHost.h \
    $$PWD/include/MetadataLookup/MetadataLookupHost.h \
    $$PWD/include/MusicLibrary/MusicLibraryDAOHost.h \
    $$PWD/include/MusicLibrary/MusicTagParserHost.h \
#    $$PWD/include/OutPut/OutPutHost.h \
    $$PWD/MusicLibrary/LocalMusicScannerThread.h \
    $$PWD/include/AudioMetaObject.h \
#    $$PWD/include/channelmap.h \
    $$PWD/include/libphoenixplayer_global.h \
    $$PWD/AudioMetaObject_p.h \
    $$PWD/include/LibPhoenixPlayerMain.h \
    $$PWD/include/AudioMetaObjectKeys.h \
    $$PWD/include/AudioMetaGroupObject.h \
    $$PWD/include/PlayerCore/MusicQueue.h \
    $$PWD/include/MediaResource.h \
    $$PWD/include/LocalFileMediaResource.h

SOURCES += \
    $$PWD/MetadataLookup/BaseNetworkLookup.cpp \
    $$PWD/MetadataLookup/IMetadataLookup.cpp \
    $$PWD/MetadataLookup/LyricsParser.cpp \
#    $$PWD/MusicLibrary/DiskLookup.cpp \
    $$PWD/MusicLibrary/MusicLibraryManager.cpp \
#    $$PWD/MusicLibrary/TagParserManager.cpp \
    $$PWD/PPSettings.cpp \
    $$PWD/PPUtility.cpp \
    $$PWD/PlayerCore/PlayListMeta.cpp \
    $$PWD/PlayerCore/PlayListMetatMgr.cpp \
    $$PWD/PlayerCore/PlayListObject.cpp \
    $$PWD/PlayerCore/PlayerCore.cpp \
    $$PWD/BaseMediaObject.cpp \
    $$PWD/IMusicTagParser.cpp \
    $$PWD/PlayerCore/RecentPlayedQueue.cpp \
    $$PWD/PluginLoader.cpp \
    $$PWD/PluginHost.cpp \
    $$PWD/AddonMgr.cpp \
#    $$PWD/MusicLibrary/AsyncDiskLookup.cpp \
#    $$PWD/MusicLibrary/AsyncTagParserMgrWrapper.cpp \
    $$PWD/MusicLibrary/LocalMusicScanner.cpp \
    $$PWD/MetadataLookup/MetadataLookupMgr.cpp \
    $$PWD/MetadataLookup/MetadataLookupMgrWrapper.cpp \
#    $$PWD/Decoder/IDecoder.cpp \
#    $$PWD/AudioParameters.cpp \
#    $$PWD/OutPut/IOutPut.cpp \
#    $$PWD/channelmap.cpp \
    $$PWD/include/Backend/BaseVolume.cpp \
    $$PWD/PlayerCore/VolumeControl.cpp \
#    $$PWD/include/Backend/SoftVolume.cpp \
    $$PWD/EqualizerMgr.cpp \
    $$PWD/PlayerCore/PlayListFormat.cpp \
    $$PWD/PlayerCore/M3uPlayListFormat.cpp \
#    $$PWD/PlayerCore/PlayQueueMgr.cpp \
    $$PWD/MusicLibrary/LocalMusicScannerThread.cpp \
    $$PWD/AudioMetaObject.cpp \
    $$PWD/AudioMetaObject_p.cpp \
    $$PWD/LibPhoenixPlayerMain.cpp \
    $$PWD/libphoenixplayer_global.cpp \
    $$PWD/AudioMetaGroupObject.cpp \
    $$PWD/PlayerCore/MusicQueue.cpp \
    $$PWD/MediaResource.cpp \
    $$PWD/LocalFileMediaResource.cpp










