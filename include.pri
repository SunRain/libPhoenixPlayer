
#include (libPhoenixPlayer.pri)

INCLUDEPATH += $$PWD/libPhoenixPlayer/Core/include
DEPENDPATH += $$PWD/libPhoenixPlayer/Core/include

HEADERS += \
#    $$PWD/libPhoenixPlayer/Core/include/Common.h \
#    $$PWD/libPhoenixPlayer/Core/include/Settings.h \
    $$PWD/libPhoenixPlayer/Core/include/AudioMetaObject.h \
#    $$PWD/libPhoenixPlayer/Core/include/Utility.h \
    $$PWD/libPhoenixPlayer/Core/include/Backend/BaseMediaObject.h \
    $$PWD/libPhoenixPlayer/Core/include/Backend/IPlayBackend.h \
    $$PWD/libPhoenixPlayer/Core/include/MusicLibrary/IMusicTagParser.h \
#    $$PWD/libPhoenixPlayer/Core/include/MusicLibrary/IPlayListDAO.h \
    $$PWD/libPhoenixPlayer/Core/include/PluginLoader.h \
    $$PWD/libPhoenixPlayer/Core/include/MetadataLookup/IMetadataLookup.h \
    $$PWD/libPhoenixPlayer/Core/include/MetadataLookup/BaseNetworkLookup.h \
    $$PWD/libPhoenixPlayer/Core/include/MetadataLookup/LyricsParser.h \
    $$PWD/libPhoenixPlayer/Core/include/MusicLibrary/MusicLibraryManager.h \
    $$PWD/libPhoenixPlayer/Core/include/SingletonPointer_p.h \
    $$PWD/libPhoenixPlayer/Core/include/SingletonPointer_sf.h \
    $$PWD/libPhoenixPlayer/Core/include/SingletonPointer.h \
    $$PWD/libPhoenixPlayer/Core/include/PluginHost.h \
    $$PWD/libPhoenixPlayer/Core/include/AddonMgr.h \
    $$PWD/libPhoenixPlayer/Core/include/MetadataLookup/MetadataLookupMgr.h \
    $$PWD/libPhoenixPlayer/Core/include/MetadataLookup/MetadataLookupMgrWrapper.h \
#    $$PWD/libPhoenixPlayer/Core/include/Buffer.h \
#    $$PWD/libPhoenixPlayer/Core/include/AudioParameters.h \
#    $$PWD/libPhoenixPlayer/Core/include/Decoder/IDecoder.h \
#    $$PWD/libPhoenixPlayer/Core/include/OutPut/IOutPut.h \
#    $$PWD/libPhoenixPlayer/Core/include/Backend/SoftVolume.h \
    $$PWD/libPhoenixPlayer/Core/include/Backend/BaseVolume.h \
#    $$PWD/libPhoenixPlayer/Core/include/OutPut/OutPutHost.h \
#    $$PWD/libPhoenixPlayer/Core/include/Decoder/DecoderHost.h \
    $$PWD/libPhoenixPlayer/Core/include/MusicLibrary/LocalMusicScanner.h \
    $$PWD/libPhoenixPlayer/Core/include/PlayerCore/VolumeControl.h \
#    $$PWD/libPhoenixPlayer/Core/include/PlayerCore/PlayListMgr.h \
    $$PWD/libPhoenixPlayer/Core/include/PlayerCore/PlayerCore.h \
    $$PWD/libPhoenixPlayer/Core/include/Backend/BackendHost.h \
    $$PWD/libPhoenixPlayer/Core/include/MetadataLookup/MetadataLookupHost.h \
    $$PWD/libPhoenixPlayer/Core/include/MusicLibrary/MusicLibraryDAOHost.h \
    $$PWD/libPhoenixPlayer/Core/include/MusicLibrary/MusicTagParserHost.h \
    $$PWD/libPhoenixPlayer/Core/include/BaseObject.h \
    $$PWD/libPhoenixPlayer/Core/include/EqualizerMgr.h \
    $$PWD/libPhoenixPlayer/Core/include/libphoenixplayer_global.h \
    $$PWD/libPhoenixPlayer/Core/include/LibPhoenixPlayerMain.h

contains (CONFIG, WITH_QML_LIB) {
    INCLUDEPATH += $$PWD/libPhoenixPlayer/QtQuick/include
    DEPENDPATH += $$PWD/libPhoenixPlayer/QtQuick/include

    HEADERS += \
        $$PWD/libPhoenixPlayer/QtQuick/include/LyricsModel.h \
        $$PWD/libPhoenixPlayer/QtQuick/include/CircleImage.h \
        $$PWD/libPhoenixPlayer/QtQuick/include/CoverCircleImage.h \
        $$PWD/libPhoenixPlayer/QtQuick/include/PathListModel.h \
        $$PWD/libPhoenixPlayer/QtQuick/include/TrackGroupModel.h \
        $$PWD/libPhoenixPlayer/QtQuick/include/AddonListModel.h \
        $$PWD/libPhoenixPlayer/QtQuick/include/PluginListModel.h
}
