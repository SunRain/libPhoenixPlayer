
QT += network core widgets

include(3rdparty/QCurl/src/qcurl.pri)

win32 {
    COPY = copy /y
} else {
    COPY = cp -f
}

CHK_DTK_PLATFORM = \
    pkg-config --exists dtkwidget && \
    if [ $? -eq 0 ]; then \
        $$COPY $$PWD/BuildConfigDeepin.enable $$PWD/BuildConfigDeepin.pri; \
    else \
        $$COPY $$PWD/BuildConfigDeepin.disable $$PWD/BuildConfigDeepin.pri; \
    fi;

system($$CHK_DTK_PLATFORM)

include($$PWD/BuildConfigDeepin.pri)

INCLUDEPATH += \
    $$PWD \
    $$PWD/include \
    $$PWD/3rdparty/CuteLogger/include

DEPENDPATH += \
    $$PWD \
    $$PWD/include \
    $$PWD/3rdparty/CuteLogger/include

DISTFILES += \
    $$PWD/BuildConfigDeepin.disable \
    $$PWD/BuildConfigDeepin.enable

HEADERS += \
    $$PWD/3rdparty/CuteLogger/include/AbstractAppender.h \
    $$PWD/3rdparty/CuteLogger/include/AbstractStringAppender.h \
    $$PWD/3rdparty/CuteLogger/include/ConsoleAppender.h \
    $$PWD/3rdparty/CuteLogger/include/CuteLogger_global.h \
    $$PWD/3rdparty/CuteLogger/include/FileAppender.h \
    $$PWD/3rdparty/CuteLogger/include/Logger.h \
    $$PWD/3rdparty/CuteLogger/include/RollingFileAppender.h \
    $$PWD/include/ApplicationManager.h \
    $$PWD/include/BasePlugin.h \
    $$PWD/include/DataProvider/IDataProvider.h \
    $$PWD/include/DataProvider/TinySpectrumDataProvider.h \
    $$PWD/include/LogManager.h \
    $$PWD/include/DataProvider/ISpectrumGenerator.h \
    $$PWD/include/PPCommon.h \
    $$PWD/include/PPUtility.h \
    $$PWD/include/PlayerCore/PlayListMeta.h \
    $$PWD/include/PlayerCore/PlayListMetaMgr.h \
    $$PWD/include/PlayerCore/PlayListObject.h \
    $$PWD/include/PlayerCore/RecentPlayedQueue.h \
    $$PWD/include/PluginLoader.h \
    $$PWD/include/PPSettings.h \
    $$PWD/include/PluginMgr.h \
    $$PWD/include/SingletonPointer_p.h \
    $$PWD/include/SingletonPointer_sf.h \
    $$PWD/include/SingletonPointer.h \
    $$PWD/include/Backend/BaseMediaObject.h \
    $$PWD/include/Backend/IPlayBackend.h \
    $$PWD/include/MetadataLookup/BaseNetworkLookup.h \
    $$PWD/include/DataProvider/IMetadataLookup.h \
    $$PWD/include/MetadataLookup/LyricsParser.h \
    $$PWD/include/DataProvider/IMusicTagParser.h \
    $$PWD/include/MusicLibrary/IMusicLibraryDAO.h \
    $$PWD/include/MusicLibrary/MusicLibraryManager.h \
    $$PWD/include/PlayerCore/PlayerCore.h \
    $$PWD/include/AddonMgr.h \
    $$PWD/include/MusicLibrary/LocalMusicScanner.h \
    $$PWD/include/MetadataLookup/MetadataLookupMgr.h \
    $$PWD/include/MetadataLookup/MetadataLookupMgrWrapper.h \
    $$PWD/include/Backend/BaseVolume.h \
    $$PWD/include/PlayerCore/VolumeControl.h \
    $$PWD/include/BaseObject.h \
    $$PWD/include/Backend/BaseVisual.h \
    $$PWD/include/EqualizerMgr.h \
    $$PWD/include/PlayerCore/PlayListFormat.h \
    $$PWD/PlayerCore/M3uPlayListFormat.h \
    $$PWD/MusicLibrary/LocalMusicScannerThread.h \
    $$PWD/include/AudioMetaObject.h \
    $$PWD/include/UserInterface/IUserInterface.h \
    $$PWD/include/UserInterface/UserInterfaceMgr.h \
    $$PWD/include/libphoenixplayer_global.h \
    $$PWD/AudioMetaObject_p.h \
    $$PWD/include/LibPhoenixPlayerMain.h \
    $$PWD/include/AudioMetaObjectKeys.h \
    $$PWD/include/AudioMetaGroupObject.h \
    $$PWD/include/PlayerCore/MusicQueue.h \
    $$PWD/include/MediaResource.h \
    $$PWD/include/LocalFileMediaResource.h \
    $$PWD/private/EqualizerMgrInternal.h \
    $$PWD/private/InternalSingletonObjectBase.h \
    $$PWD/private/LocalMusicScannerInternal.h \
    $$PWD/private/MusicLibraryManagerInternal.h \
    $$PWD/private/PPSettingsInternal.h \
    $$PWD/private/PlayListMetaMgrInternal.h \
    $$PWD/private/PlayerCoreInternal.h \
    $$PWD/private/PluginMgrInternal.h \
    $$PWD/private/SingletonObjectFactory.h \
    $$PWD/private/UserInterfaceMgrInternal.h \
    $$PWD/private/VolumeControlInternal.h

SOURCES += \
    $$PWD/3rdparty/CuteLogger/src/AbstractAppender.cpp \
    $$PWD/3rdparty/CuteLogger/src/AbstractStringAppender.cpp \
    $$PWD/3rdparty/CuteLogger/src/ConsoleAppender.cpp \
    $$PWD/3rdparty/CuteLogger/src/FileAppender.cpp \
    $$PWD/3rdparty/CuteLogger/src/Logger.cpp \
    $$PWD/3rdparty/CuteLogger/src/RollingFileAppender.cpp \
    $$PWD/ApplicationManager.cpp \
    $$PWD/BasePlugin.cpp \
    $$PWD/DataProvider/IDataProvider.cpp \
    $$PWD/DataProvider/TinySpectrumDataProvider.cpp \
    $$PWD/LogManager.cpp \
    $$PWD/MetadataLookup/BaseNetworkLookup.cpp \
    $$PWD/DataProvider/IMetadataLookup.cpp \
    $$PWD/MetadataLookup/LyricsParser.cpp \
    $$PWD/MusicLibrary/MusicLibraryManager.cpp \
    $$PWD/PPSettings.cpp \
    $$PWD/PPUtility.cpp \
    $$PWD/PlayerCore/PlayListMeta.cpp \
    $$PWD/PlayerCore/PlayListMetaMgr.cpp \
    $$PWD/PlayerCore/PlayListObject.cpp \
    $$PWD/PlayerCore/PlayerCore.cpp \
    $$PWD/BaseMediaObject.cpp \
    $$PWD/IMusicTagParser.cpp \
    $$PWD/PlayerCore/RecentPlayedQueue.cpp \
    $$PWD/PluginLoader.cpp \
    $$PWD/PluginHost.cpp \
    $$PWD/AddonMgr.cpp \
    $$PWD/MusicLibrary/LocalMusicScanner.cpp \
    $$PWD/MetadataLookup/MetadataLookupMgr.cpp \
    $$PWD/MetadataLookup/MetadataLookupMgrWrapper.cpp \
    $$PWD/PluginMgr.cpp \
    $$PWD/UserInterface/IUserInterface.cpp \
    $$PWD/UserInterface/UserInterfaceMgr.cpp \
    $$PWD/include/Backend/BaseVolume.cpp \
    $$PWD/PlayerCore/VolumeControl.cpp \
    $$PWD/EqualizerMgr.cpp \
    $$PWD/PlayerCore/PlayListFormat.cpp \
    $$PWD/PlayerCore/M3uPlayListFormat.cpp \
    $$PWD/MusicLibrary/LocalMusicScannerThread.cpp \
    $$PWD/AudioMetaObject.cpp \
    $$PWD/AudioMetaObject_p.cpp \
    $$PWD/LibPhoenixPlayerMain.cpp \
    $$PWD/libphoenixplayer_global.cpp \
    $$PWD/AudioMetaGroupObject.cpp \
    $$PWD/PlayerCore/MusicQueue.cpp \
    $$PWD/MediaResource.cpp \
    $$PWD/LocalFileMediaResource.cpp \
    $$PWD/private/EqualizerMgrInternal.cpp \
    $$PWD/private/InternalSingletonObjectBase.cpp \
    $$PWD/private/LocalMusicScannerInternal.cpp \
    $$PWD/private/MusicLibraryManagerInternal.cpp \
    $$PWD/private/PPSettingsInternal.cpp \
    $$PWD/private/PlayListMetaMgrInternal.cpp \
    $$PWD/private/PlayerCoreInternal.cpp \
    $$PWD/private/PluginMgrInternal.cpp \
    $$PWD/private/SingletonObjectFactory.cpp \
    $$PWD/private/UserInterfaceMgrInternal.cpp \
    $$PWD/private/VolumeControlInternal.cpp










