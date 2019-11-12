#ifndef PLUGINMGRINTERNAL_H
#define PLUGINMGRINTERNAL_H

#include <QObject>
#include <QSharedPointer>
#include <QCryptographicHash>

#include "libphoenixplayer_global.h"
#include "PluginMgr.h"

class QSettings;
namespace PhoenixPlayer {

class PPSettingsInternal;
class LIBPHOENIXPLAYER_EXPORT PluginMgrInternal : public QObject
{
    Q_OBJECT
public:
    explicit PluginMgrInternal(QSharedPointer<PPSettingsInternal> set, QObject *parent = Q_NULLPTR);
    virtual ~PluginMgrInternal();

    void addPluginPath(const QString &path);

    void reload();

    QList<PluginMetaData> pluginMetaDataList(BasePlugin::PluginType type) const;

    QList<PluginMetaData> pluginMetaDataList(DataProvider::IDataProvider::SupportedTypes type) const;

    inline QList<PluginMetaData> metaDataList() const
    {
        return m_metaList;
    }

    inline QList<PluginMetaData> dataProviderList() const
    {
        return m_dataProviderList;
    }

    void update(const PluginMetaData &data);

    void setUsedMusicLibraryDAO(const PluginMetaData &data);

    inline PluginMetaData usedMusicLibraryDAO() const
    {
        return m_usedMusicLibraryDAO;
    }

    void setUsedPlayBackend(const PluginMetaData &data);

    inline PluginMetaData usedPlayBackend() const
    {
        return m_usedPlayBackend;
    }

protected:
    void initPluginByPath(const QString &path);

private:
    inline QString groupName(const QString &absFile) const
    {
        return QCryptographicHash::hash(absFile.toUtf8(), QCryptographicHash::Md5).toHex();
    }

    void write(const PluginMetaData &data);

    void initFallbackMusicLibraryDAO();

    void initFallbackPlayBackend();

private:
    QSharedPointer<PPSettingsInternal>  m_setting;
    QStringList                         m_pluginPaths;
//    QStringList                         m_registeredPlugins;
    QList<PluginMetaData>               m_metaList;
    QList<PluginMetaData>               m_dataProviderList;

    PluginMetaData                      m_usedMusicLibraryDAO;
    PluginMetaData                      m_usedPlayBackend;

};

} //PhoenixPlayer
#endif // PLUGINMGRINTERNAL_H
