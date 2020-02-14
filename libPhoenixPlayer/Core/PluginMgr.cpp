#include "PluginMgr.h"

#include <QPluginLoader>
#include <QFile>
#include <QDebug>

#include "private/SingletonObjectFactory.h"
#include "private/PluginMgrInternal.h"

namespace PhoenixPlayer {


PluginMetaData::PluginMetaData()
{

}

PluginMetaData::PluginMetaData(const PluginMetaData &other)
{
    this->enabled = other.enabled;
    this->type = other.type;
    this->libraryFile = other.libraryFile;
    this->checksum = other.checksum;
    this->extraData = other.extraData;
    this->property = other.property;
}

PluginMetaData &PluginMetaData::operator =(const PluginMetaData &other)
{
    this->enabled = other.enabled;
    this->type = other.type;
    this->libraryFile = other.libraryFile;
    this->checksum = other.checksum;
    this->extraData = other.extraData;
    this->property = other.property;
    return *this;
}

bool PluginMetaData::isValid(const PluginMetaData &data)
{
    return !data.libraryFile.isEmpty() && (data.type != BasePlugin::PluginTypeUndefined);
}

PluginMgr::PluginMgr(QObject *parent)
    : QObject(parent)
{
    m_internal = SingletonObjectFactory::instance()->pluginMgrInternal();
}

PluginMgr::~PluginMgr()
{

}

void PluginMgr::addPluginPath(const QString &path)
{
    m_internal->addPluginPath(path);
}

void PluginMgr::reload()
{
    m_internal->reload();
}

QObject *PluginMgr::instance(const QString &libraryFile)
{
    if (!QFile::exists(libraryFile)) {
        qDebug()<<Q_FUNC_INFO<<"Library file ["<<libraryFile<<"] not exists!!";
        return Q_NULLPTR;
    }
    QPluginLoader loader(libraryFile);
    return loader.instance();
}

QObject *PluginMgr::instance(const PluginMetaData &data)
{
    return instance(data.libraryFile);
}

void PluginMgr::unload(const PluginMetaData &data)
{
    unload(data.libraryFile);
}

void PluginMgr::unload(const QString &libraryFile)
{
    if (!QFile::exists(libraryFile)) {
        qDebug()<<Q_FUNC_INFO<<"Library file ["<<libraryFile<<"] not exists!!";
        return;
    }
    QPluginLoader loader(libraryFile);
    if (!loader.isLoaded()) {
        qDebug()<<Q_FUNC_INFO<<"Library file ["<<libraryFile<<"] is not loaderd!!";
        return;
    }
    if (!loader.unload()) {
        qDebug()<<Q_FUNC_INFO<<"Can't unload file ["
                 <<libraryFile
                 <<"] with error "
                 <<loader.errorString();
    }
}

QList<PluginMetaData> PluginMgr::pluginMetaDataList(BasePlugin::PluginType type) const
{
    return m_internal->pluginMetaDataList(type);
}

QList<PluginMetaData> PluginMgr::dataProviderList(DataProvider::IDataProvider::SupportedTypes type) const
{
    return m_internal->dataProviderList(type);
}

//QList<PluginMetaData> PluginMgr::pluginMetaDataList(DataProvider::IDataProvider::SupportedTypes type) const
//{
//    return m_internal->pluginMetaDataList(type);
//}

QList<PluginMetaData> PluginMgr::metaDataList() const
{
    return m_internal->metaDataList();
}

QList<PluginMetaData> PluginMgr::dataProviderList() const
{
    return m_internal->dataProviderList();
}

void PluginMgr::update(const PluginMetaData &data)
{
    m_internal->update(data);
}

void PluginMgr::setUsedMusicLibraryDAO(const PluginMetaData &data)
{
    m_internal->setUsedMusicLibraryDAO(data);
}

PluginMetaData PluginMgr::usedMusicLibraryDAO() const
{
    return m_internal->usedMusicLibraryDAO();
}

void PluginMgr::setUsedPlayBackend(const PluginMetaData &data)
{
    m_internal->setUsedPlayBackend(data);
}

PluginMetaData PluginMgr::usedPlayBackend() const
{
    return m_internal->usedPlayBackend();
}








} //namespace PhoenixPlayer
