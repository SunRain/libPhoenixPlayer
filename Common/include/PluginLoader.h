#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include <QHash>
#include <QList>

namespace PhoenixPlayer {

namespace PlayBackend {
class IPlayBackend;
class IMusicTagParser;
}

namespace MusicLibrary {
class IPlayListDAO;
class IMusicTagParser;
}

namespace MetadataLookup {
class IMetadataLookup;
}
class PluginLoader : public QObject
{
    Q_OBJECT
    Q_ENUMS(PluginType)
public:
    //不使用强类型枚举,因为QHash不支持强类型枚举
    enum /*class*/ PluginType {
        TypeAll = 0x0,
        //为保证后续兼容,TypePlayBackend必须为第二个项
        TypePlayBackend,            //播放后端
        TypePlayListDAO,            //音乐库存储后端
        TypeMusicTagParser,         //音乐Tag解析
        TypeMetadataLookup,           //metadata查询
        TypeLastFlag                //最后一个标记,为保证兼容,所有后续添加的枚举必须在此项之前
    };
    static PluginLoader *getInstance();
    explicit PluginLoader(QObject *parent = 0);
    virtual ~PluginLoader();

    void setPluginPath(PluginType type, const QString &path);

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
    QStringList getPluginNames(PluginType type);

protected:
    void initPlugins(PluginType type);
signals:
    ///
    /// \brief signalPluginChanged setNewPlugin后如果新插件名和当前使用的插件名不同,则发送此信号
    /// \param type
    ///
    void signalPluginChanged(PluginType type);

public slots:
    ///
    /// \brief setNewPlugin 设置一个新插件,如果新插件名和当前使用的插件名相同,则忽略设置动作
    /// \param type
    /// \param newPluginName
    ///
     void setNewPlugin(PluginType type, const QString &newPluginName);

private:
     void initPlayBackendPlugin();
     void initPlayListDaoPlugin();
     void initMusicTagParserPlugin();
     void initLyricsLookupPlugin();
private:
      QList <PlayBackend::IPlayBackend*> mPlayBackendList;
      QList <MusicLibrary::IPlayListDAO*> mPlayListDAOList;
      QList <MusicLibrary::IMusicTagParser*> mMusicTagParserList;
      QList <MetadataLookup::IMetadataLookup*> mMetaLookupList;

      QHash<PluginType, int> mCurrentPluginIndex;
      QHash<PluginType, QString> mCurrentPluginName;
      QHash<PluginType, QString> mPluginPath;
};

} //PhoenixPlayer
#endif // PLUGINLOADER_H
