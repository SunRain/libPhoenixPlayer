
#include (libPhoenixPlayer.pri)

INCLUDEPATH += $$PWD/Core/include \
    $$PWD/Core/3rdparty/QCurl/src \
    $$PWD/Core/3rdparty/CuteLogger/include

DEPENDPATH += $$PWD/Core/include \
    $$PWD/Core/3rdparty/QCurl/src \
    $$PWD/Core/3rdparty/CuteLogger/include

HEADERS += \
    $$PWD/Core/3rdparty/CuteLogger/include/AbstractAppender.h \
    $$PWD/Core/3rdparty/CuteLogger/include/AbstractStringAppender.h \
    $$PWD/Core/3rdparty/CuteLogger/include/ConsoleAppender.h \
    $$PWD/Core/3rdparty/CuteLogger/include/CuteLogger_global.h \
    $$PWD/Core/3rdparty/CuteLogger/include/FileAppender.h \
    $$PWD/Core/3rdparty/CuteLogger/include/Logger.h \
    $$PWD/Core/3rdparty/CuteLogger/include/RollingFileAppender.h \
    $$PWD/Core/3rdparty/QCurl/src/CurlEasyHandleInitializtionClass.h \
    $$PWD/Core/3rdparty/QCurl/src/CurlGlobalConstructor.h \
    $$PWD/Core/3rdparty/QCurl/src/CurlMultiHandleProcesser.h \
    $$PWD/Core/3rdparty/QCurl/src/QCNetworkAccessManager.h \
    $$PWD/Core/3rdparty/QCurl/src/QCNetworkAccessManager_p.h \
    $$PWD/Core/3rdparty/QCurl/src/QCNetworkAsyncDataPostReply.h \
    $$PWD/Core/3rdparty/QCurl/src/QCNetworkAsyncHttpGetReply.h \
    $$PWD/Core/3rdparty/QCurl/src/QCNetworkAsyncHttpGetReply_p.h \
    $$PWD/Core/3rdparty/QCurl/src/QCNetworkAsyncHttpHeadReply.h \
    $$PWD/Core/3rdparty/QCurl/src/QCNetworkAsyncHttpHeadReply_p.h \
    $$PWD/Core/3rdparty/QCurl/src/QCNetworkAsyncReply.h \
    $$PWD/Core/3rdparty/QCurl/src/QCNetworkAsyncReply_p.h \
    $$PWD/Core/3rdparty/QCurl/src/QCNetworkRequest.h \
    $$PWD/Core/3rdparty/QCurl/src/QCNetworkSyncReply.h \
    $$PWD/Core/3rdparty/QCurl/src/QCUtility.h \
    $$PWD/Core/include/ApplicationManager.h \
    $$PWD/Core/include/AudioMetaGroupObject.h \
    $$PWD/Core/include/AudioMetaObject.h \
    $$PWD/Core/include/AudioMetaObjectKeys.h \
    $$PWD/Core/include/Backend/BackendHost.h \
    $$PWD/Core/include/Backend/BaseMediaObject.h \
    $$PWD/Core/include/Backend/BaseVisual.h \
    $$PWD/Core/include/Backend/BaseVolume.h \
    $$PWD/Core/include/Backend/IPlayBackend.h \
    $$PWD/Core/include/BaseObject.h \
    $$PWD/Core/include/BasePlugin.h \
    $$PWD/Core/include/DataProvider/IDataProvider.h \
    $$PWD/Core/include/DataProvider/IMetadataLookup.h \
    $$PWD/Core/include/DataProvider/IMusicTagParser.h \
    $$PWD/Core/include/DataProvider/ISpectrumGenerator.h \
    $$PWD/Core/include/DataProvider/TinySpectrumDataProvider.h \
    $$PWD/Core/include/EqualizerMgr.h \
    $$PWD/Core/include/LibPhoenixPlayerMain.h \
    $$PWD/Core/include/LocalFileMediaResource.h \
    $$PWD/Core/include/LogManager.h \
    $$PWD/Core/include/MediaResource.h \
    $$PWD/Core/include/MetadataLookup/BaseNetworkLookup.h \
    $$PWD/Core/include/MetadataLookup/LyricsParser.h \
    $$PWD/Core/include/MetadataLookup/MetadataLookupMgr.h \
    $$PWD/Core/include/MetadataLookup/MetadataLookupMgrWrapper.h \
    $$PWD/Core/include/PPCommon.h \
    $$PWD/Core/include/PPSettings.h \
    $$PWD/Core/include/PPUtility.h \
    $$PWD/Core/include/PlayerCore/MusicQueue.h \
    $$PWD/Core/include/PlayerCore/PlayListMeta.h \
    $$PWD/Core/include/PlayerCore/PlayListMetaMgr.h \
    $$PWD/Core/include/PlayerCore/PlayListObject.h \
    $$PWD/Core/include/PlayerCore/PlayerCore.h \
    $$PWD/Core/include/PlayerCore/RecentPlayedQueue.h \
    $$PWD/Core/include/PlayerCore/VolumeControl.h \
#    $$PWD/Core/include/PluginHost.h \
#    $$PWD/Core/include/PluginLoader.h \
    $$PWD/Core/include/PluginMgr.h \
    $$PWD/Core/include/SingletonPointer.h \
    $$PWD/Core/include/SingletonPointer_p.h \
    $$PWD/Core/include/SingletonPointer_sf.h \
    $$PWD/Core/include/UserInterface/IUserInterface.h \
    $$PWD/Core/include/UserInterface/UserInterfaceMgr.h \
    $$PWD/Core/include/libphoenixplayer_global.h \
    $$PWD/Core/include/AddonMgr.h

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
