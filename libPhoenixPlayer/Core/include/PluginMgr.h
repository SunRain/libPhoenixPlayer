#ifndef PLUGINMGR_H
#define PLUGINMGR_H

#include <QObject>
#include <QSharedPointer>

#include "libphoenixplayer_global.h"
#include "BasePlugin.h"
#include "DataProvider/IDataProvider.h"

namespace PhoenixPlayer {

class LIBPHOENIXPLAYER_EXPORT PluginMetaData
{
public:
    PluginMetaData();
    PluginMetaData(const PluginMetaData &other);

    bool operator == (const PluginMetaData &other) = delete;
    bool operator != (const PluginMetaData &other) = delete;
    PluginMetaData &operator = (const PluginMetaData &other);

    bool                    enabled         = false;
    BasePlugin::PluginType  type            = BasePlugin::PluginType::PluginTypeUndefined;
    QString                 libraryFile;    // abs path of library file
    QString                 checksum;       // checksum to identify if plugin library file changed
    QVariant                extraData;      // any extra data you want to insert into
    PluginProperty          property;

    /*!
     * \brief isValid
     * The libraryFile is not empty and type is not PluginTypeUndefined if a plugin meta is valid.
     * \param data
     * \return
     */
    static bool isValid(const PluginMetaData &data);
};


class PluginMgrInternal;

/*!
 * \brief The PluginMgr class
 * Internal Singleton class
 */
class LIBPHOENIXPLAYER_EXPORT PluginMgr : public QObject
{
    Q_OBJECT
public:
    explicit PluginMgr(QObject *parent = Q_NULLPTR);
    virtual ~PluginMgr();

    void addPluginPath(const QString &path);

    void reload();

    static QObject *instance(const QString &libraryFile);

    static QObject *instance(const PluginMetaData &data);

    static void unload(const PluginMetaData &data);

    static void unload(const QString &libraryFile);

    QList<PluginMetaData> pluginMetaDataList(BasePlugin::PluginType type) const;

//    QList<PluginMetaData> pluginMetaDataList(DataProvider::IDataProvider::SupportedTypes type) const;
    QList<PluginMetaData> dataProviderList(DataProvider::IDataProvider::SupportedTypes type) const;

    QList<PluginMetaData> metaDataList() const;

    QList<PluginMetaData> dataProviderList() const;

    /*!
     * \brief update Update or append to internal list
     * \param data
     */
    void update(const PluginMetaData &data);

    void setUsedMusicLibraryDAO(const PluginMetaData &data);

    PluginMetaData usedMusicLibraryDAO() const;

    void setUsedPlayBackend(const PluginMetaData &data);

    PluginMetaData usedPlayBackend() const;

private:
    QSharedPointer<PluginMgrInternal> m_internal;
};

} //PhoenixPlayer


#endif // PLUGINMGR_H
