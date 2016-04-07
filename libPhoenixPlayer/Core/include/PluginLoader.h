#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include <QHash>
#include <QList>

#include "libphoenixplayer_global.h"
#include "Common.h"
#include "SingletonPointer.h"

class QPluginLoader;
namespace PhoenixPlayer {

namespace PlayBackend {
class BackendHost;
}

namespace MusicLibrary {
class MusicLibraryDAOHost;
}

namespace OutPut {
class OutPutHost;
}

class Settings;
class PluginHost;
class LIBPHOENIXPLAYER_EXPORT PluginLoader : public QObject
{
    Q_OBJECT
//    DECLARE_SINGLETON_POINTER(PluginLoader)
public:
    explicit PluginLoader(Settings *set, QObject *parent = 0);
    virtual ~PluginLoader();

    void addPluginPath(const QString &path);

    ///
    /// \brief getCurrentPlayBackend
    /// 如果未设置有效插件名,则返回当前列表的第一个插件,否则返回设置的插件
    /// \return
    ///
//    PlayBackend::IPlayBackend *getCurrentPlayBackend();

    PlayBackend::BackendHost *curBackendHost();
    ///
    /// \brief getCurrentPlayListDAO
    /// 如果未设置有效插件名,则返回当前列表的第一个插件,否则返回设置的插件
    /// \return
    ///
//    MusicLibrary::IMusicLibraryDAO *getCurrentLibraryDAO();

    OutPut::OutPutHost *curOutPutHost();

    ///
    /// \brief getCurrentMusicTagParser
    /// 如果未设置有效插件名,则返回当前列表的第一个插件,否则返回设置的插件
    /// \return
    ///
//    MusicLibrary::IMusicTagParser *getCurrentMusicTagParser();

    MusicLibrary::MusicLibraryDAOHost *curDAOHost();

    ///
    /// \brief getCurrentMetadataLookup
    /// 如果未设置有效插件名,则返回当前列表的第一个插件,否则返回设置的插件
    /// \return
    ///
//    MetadataLookup::IMetadataLookup *getCurrentMetadataLookup();


//    Decoder::IDecoder *getCurrentDecoder();

//    OutPut::IOutPut *getCurrentOutPut();

    ///
    /// \brief getPluginNames 返回当前所有插件的名称列表
    /// \param type
    /// \return
    ///
//    QStringList getPluginHostHashList(Common::PluginType type);

    ///
    /// \brief pluginLibraries
    /// \param type
    /// \return 某一个插件类型的所有插件文件地址
    ///
    QStringList pluginLibraries(Common::PluginType type);
protected:
    void initPluginByPath(const QString &path);
signals:
    ///
    /// \brief signalPluginChanged setNewPlugin后如果新插件名和当前使用的插件名不同,则发送此信号
    /// \param type
    ///
//    void signalPluginChanged(Common::PluginType type);
private:
     Settings *m_settings;
     PlayBackend::BackendHost *m_curBackendHost;
     OutPut::OutPutHost *m_curOutPutHost;
     MusicLibrary::MusicLibraryDAOHost *m_curDAOHost;

     QStringList m_pluginPaths;
     QHash<Common::PluginType, QString> m_libraries;

};

} //PhoenixPlayer
#endif // PLUGINLOADER_H
