#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include <QHash>
#include <QList>
#include "Common.h"

class QPluginLoader;
namespace PhoenixPlayer {

namespace PlayBackend {
class IPlayBackend;
}

namespace MusicLibrary {
class IPlayListDAO;
class IMusicTagParser;
}

namespace MetadataLookup {
class IMetadataLookup;
}

class PluginHost;
class PluginLoader : public QObject
{
    Q_OBJECT
public:
    explicit PluginLoader(QObject *parent = 0);
    virtual ~PluginLoader();

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    static PluginLoader *instance();
#endif

    void setPluginPath(Common::PluginType type, const QString &path);

    ///
    /// \brief getCurrentPlayBackend
    /// 如果未设置有效插件名,则返回当前列表的第一个插件,否则返回设置的插件
    /// \return
    ///
    PlayBackend::IPlayBackend *getCurrentPlayBackend();
    ///
    /// \brief getCurrentPlayListDAO
    /// 如果未设置有效插件名,则返回当前列表的第一个插件,否则返回设置的插件
    /// \return
    ///
    MusicLibrary::IPlayListDAO *getCurrentPlayListDAO();
    ///
    /// \brief getCurrentMusicTagParser
    /// 如果未设置有效插件名,则返回当前列表的第一个插件,否则返回设置的插件
    /// \return
    ///
    MusicLibrary::IMusicTagParser *getCurrentMusicTagParser();

    ///
    /// \brief getCurrentMetadataLookup
    /// 如果未设置有效插件名,则返回当前列表的第一个插件,否则返回设置的插件
    /// \return
    ///
    MetadataLookup::IMetadataLookup *getCurrentMetadataLookup();

    ///
    /// \brief getPluginNames 返回当前所有插件的名称列表
    /// \param type
    /// \return
    ///
    QStringList getPluginHostHashList(Common::PluginType type);

    int getPluginHostSize(Common::PluginType type);

    PluginHost *getCurrentPluginHost(Common::PluginType type);

    QList<PluginHost *> getPluginHostList(Common::PluginType type);

protected:
    void initPlugins();
signals:
    ///
    /// \brief signalPluginChanged setNewPlugin后如果新插件名和当前使用的插件名不同,则发送此信号
    /// \param type
    ///
    void signalPluginChanged(Common::PluginType type);

public slots:
    ///
    /// \brief setNewPlugin 设置一个新插件,如果新插件名和当前使用的插件名相同,则忽略设置动作
    /// \param type
    /// \param newPluginName
    ///
     void setNewPlugin(Common::PluginType type, const QString &newPluginHash);
private:
     bool isInit;
     QList<PluginHost *> mPluginHostList;
     QHash<Common::PluginType, PluginHost*> mCurrentPluginHost;
     QHash<Common::PluginType, QString> mPluginPath;
};

} //PhoenixPlayer
#endif // PLUGINLOADER_H
