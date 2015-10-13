
include (libPhoenixPlayer.pri)

INCLUDEPATH += $$PWD/libPhoenixPlayer/Core/include
DEPENDPATH += $$PWD/libPhoenixPlayer/Core/include

HEADERS += \
    $$PWD/libPhoenixPlayer/Core/include/Common.h \
    $$PWD/libPhoenixPlayer/Core/include/Settings.h \
    $$PWD/libPhoenixPlayer/Core/include/SongMetaData.h \
    $$PWD/libPhoenixPlayer/Core/include/Util.h \
    $$PWD/libPhoenixPlayer/Core/include/Backend/BaseMediaObject.h \
    $$PWD/libPhoenixPlayer/Core/include/Backend/IPlayBackend.h \
    $$PWD/libPhoenixPlayer/Core/include/MusicLibrary/IMusicTagParser.h \
    $$PWD/libPhoenixPlayer/Core/include/MusicLibrary/IPlayListDAO.h \
    $$PWD/libPhoenixPlayer/Core/include/PluginLoader.h \
    $$PWD/libPhoenixPlayer/Core/include/MetadataLookup/IMetadataLookup.h \
    $$PWD/libPhoenixPlayer/Core/include/MetadataLookup/BaseNetworkLookup.h \
    $$PWD/libPhoenixPlayer/Core/include/MetadataLookup/LyricsParser.h \
    $$PWD/libPhoenixPlayer/Core/include/MusicLibrary/MusicLibraryManager.h \
    $$PWD/libPhoenixPlayer/Core/include/Player/Player.h \
    $$PWD/libPhoenixPlayer/Core/include/SingletonPointer_p.h \
    $$PWD/libPhoenixPlayer/Core/include/SingletonPointer_sf.h \
    $$PWD/libPhoenixPlayer/Core/include/SingletonPointer.h \
    $$PWD/libPhoenixPlayer/Core/include/PluginHost.h \
    $$PWD/libPhoenixPlayer/Core/include/AddonMgr.h \
    $$PWD/libPhoenixPlayer/Core/include/MusicLibrary/LocalMusicSacnner.h \
    $$PWD/libPhoenixPlayer/Core/include/MetadataLookup/MetadataLookupMgr.h \
    $$PWD/libPhoenixPlayer/Core/include/MetadataLookup/MetadataLookupMgrWrapper.h \
    $$PWD/libPhoenixPlayer/Core/include/Buffer.h \
    $$PWD/libPhoenixPlayer/Core/include/AudioParameters.h \
    $$PWD/libPhoenixPlayer/Core/include/Decoder/IDecoder.h \
    $$PWD/libPhoenixPlayer/Core/include/OutPut/IOutPut.h \
    $$PWD/libPhoenixPlayer/Core/include/Backend/SoftVolume.h \
    $$PWD/libPhoenixPlayer/Core/include/Backend/BaseVolume.h \
    $$PWD/libPhoenixPlayer/Core/include/OutPut/OutPutHost.h \
    $$PWD/libPhoenixPlayer/Core/include/Decoder/DecoderHost.h

contains (CONFIG, WITH_QML_LIB) {
    INCLUDEPATH += $$PWD/libPhoenixPlayer/QtQuick/include
    DEPENDPATH += $$PWD/libPhoenixPlayer/QtQuick/include

    HEADERS += \
        $$PWD/libPhoenixPlayer/QtQuick/include/LyricsModel.h \
        $$PWD/libPhoenixPlayer/QtQuick/include/CircleImage.h \
        $$PWD/libPhoenixPlayer/QtQuick/include/CoverCircleImage.h \
        $$PWD/libPhoenixPlayer/QtQuick/include/MusicLibraryListModel.h \
        $$PWD/libPhoenixPlayer/QtQuick/include/PathListModel.h \
        $$PWD/libPhoenixPlayer/QtQuick/include/TrackGroupModel.h \
        $$PWD/libPhoenixPlayer/QtQuick/include/AddonListModel.h \
        $$PWD/libPhoenixPlayer/QtQuick/include/PluginListModel.h
}
