#include "PluginListModel.h"

#include <QDebug>

#include "SingletonPointer.h"
#include "PluginLoader.h"
#include "PluginHost.h"
#include "Common.h"
#include "Decoder/DecoderHost.h"
#include "MetadataLookup/MetadataLookupHost.h"
#include "MusicLibrary/MusicLibraryDAOHost.h"
#include "MusicLibrary/MusicTagParserHost.h"
#include "OutPut/OutPutHost.h"
#include "Backend/BackendHost.h"

namespace PhoenixPlayer {
namespace QmlPlugin {
PluginListModel::PluginListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_pluginLoader = phoenixPlayerLib->pluginLoader ();//PluginLoader::instance ();

    QStringList libs = m_pluginLoader->pluginLibraries (Common::PluginDecoder);
    foreach (QString s, libs) {
        PluginHost *h = new Decoder::DecoderHost(s);
        if (h->isValid ())
            m_hosts.append (h);
    }

    libs.clear ();
    libs = m_pluginLoader->pluginLibraries (Common::PluginMetadataLookup);
    foreach (QString s, libs) {
        PluginHost *h = new MetadataLookup::MetadataLookupHost(s);
        if (h->isValid ())
            m_hosts.append (h);
    }

    libs.clear ();
    libs = m_pluginLoader->pluginLibraries (Common::PluginMusicLibraryDAO);
    foreach (QString s, libs) {
        PluginHost *h = new MusicLibrary::MusicLibraryDAOHost(s);
        if (h->isValid ())
            m_hosts.append (h);
    }

    libs.clear ();
    libs = m_pluginLoader->pluginLibraries (Common::PluginMusicTagParser);
    foreach (QString s, libs) {
        PluginHost *h = new MusicLibrary::MusicTagParserHost(s);
        if (h->isValid ())
            m_hosts.append (h);
    }

    libs.clear ();
    libs = m_pluginLoader->pluginLibraries (Common::PluginOutPut);
    foreach (QString s, libs) {
        PluginHost *h = new OutPut::OutPutHost(s);
        if (h->isValid ())
            m_hosts.append (h);
    }

    libs.clear ();
    libs = m_pluginLoader->pluginLibraries (Common::PluginPlayBackend);
    foreach (QString s, libs) {
        PluginHost *h = new PlayBackend::BackendHost(s);
        if (h->isValid ())
            m_hosts.append (h);
    }
}

PluginListModel::~PluginListModel()
{
    if (!m_hosts.isEmpty ()) {
        qDeleteAll(m_hosts);
        m_hosts.clear ();
    }
}

int PluginListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_hosts.size ();
}

QVariant PluginListModel::data(const QModelIndex &index, int role) const
{
    if (index.row () < 0 || index.row () >= m_hosts.size ()) {
        return QVariant();
    }
    if (m_hosts.isEmpty ()) {
        qDebug()<<Q_FUNC_INFO<<"host size is empty";
        return QVariant();
    }

    switch (role) {
    case ModelRoles::RoleConfigFile:
        return m_hosts.at (index.row ())->configFile ();
    case ModelRoles::RoleDescription:
        return m_hosts.at (index.row ())->description ();
    case ModelRoles::RoleName:
        return m_hosts.at (index.row ())->name ();
    case ModelRoles::RoleVersion:
        return m_hosts.at (index.row ())->version ();
    case ModelRoles::RolePluginType:
        return m_hosts.at (index.row ())->type ();
    case ModelRoles::RoleLibraryFile:
        return m_hosts.at (index.row ())->libraryFile ();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PluginListModel::roleNames() const
{
    QHash<int, QByteArray> role;
    role.insert (ModelRoles::RoleConfigFile, "configFile");
    role.insert (ModelRoles::RoleDescription, "description");
    role.insert (ModelRoles::RoleName, "name");
    role.insert (ModelRoles::RoleVersion, "version");
    role.insert (ModelRoles::RolePluginType, "type");
    role.insert (ModelRoles::RoleLibraryFile, "hash");
    return role;
}

} //QmlPlugin
} //PhoenixPlayer
