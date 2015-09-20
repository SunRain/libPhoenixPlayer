#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include <QHash>
#include <QList>
#include "Common.h"
#include "SingletonPointer.h"

class QPluginLoader;
namespace PhoenixPlayer {

namespace PlayBackend {
//class IPlayBackend;
class BackendHost;
}

namespace MusicLibrary {
class MusicLibraryDAOHost;
}

//namespace MetadataLookup {
////class IMetadataLookup;
//}

//namespace Decoder {
////class IDecoder;
//class DecoderHost;
//}

namespace OutPut {
//class IOutPut;
class OutPutHost;
}

class Settings;
class PluginHost;
class PluginLoader : public QObject
{
    Q_OBJECT
    DECLARE_SINGLETON_POINTER(PluginLoader)
public:
    virtual ~PluginLoader();

//#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
//    static PluginLoader *instance();
//#endif

//    void setPluginPath(Common::PluginType type, const QString &path);
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

//    int getPluginHostSize(Common::PluginType type);

//    PluginHost *getCurrentPluginHost(Common::PluginType type);

//    PluginHost *getPluginHostByHash(const QString &hostHash);

//    QList<PluginHost *> getPluginHostList(Common::PluginType type);

protected:
    void initPluginByPath(const QString &path);
//    void initPluginByPath(const QString &path);
//    explicit PluginLoader(QObject *parent = 0);
signals:
    ///
    /// \brief signalPluginChanged setNewPlugin后如果新插件名和当前使用的插件名不同,则发送此信号
    /// \param type
    ///
//    void signalPluginChanged(Common::PluginType type);

public slots:
    ///
    /// \brief setNewPlugin 设置一个新插件,如果新插件名和当前使用的插件名相同,则忽略设置动作
    /// \param type
    /// \param newPluginName
    ///
//     void setNewPlugin(Common::PluginType type, const QString &newPluginHash);
private:
//     bool m_isInit;
//     QList<PluginHost *> m_pluginHostList;
//     QHash<Common::PluginType, PluginHost*> m_currentPluginHost;
//     QHash<Common::PluginType, QString> m_pluginPath;
     Settings *m_settings;
     PlayBackend::BackendHost *m_curBackendHost;
     OutPut::OutPutHost *m_curOutPutHost;
     MusicLibrary::MusicLibraryDAOHost *m_curDAOHost;

     QStringList m_pluginPaths;
     QHash<Common::PluginType, QString> m_libraries;

};

} //PhoenixPlayer
#endif // PLUGINLOADER_H
