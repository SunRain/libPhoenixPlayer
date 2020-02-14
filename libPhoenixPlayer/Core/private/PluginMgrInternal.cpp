#include "PluginMgrInternal.h"

#include <QCoreApplication>
#include <QPluginLoader>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QSettings>
#include <QMetaObject>
#include <QMetaEnum>

#include "Logger.h"

#include "Backend/IPlayBackend.h"
#include "DataProvider/IDataProvider.h"
#include "MusicLibrary/IMusicLibraryDAO.h"
#include "Backend/IPlayBackend.h"
#include "UserInterface/IUserInterface.h"

#include "PPSettingsInternal.h"

namespace PhoenixPlayer {

//const static char *KEY_REGISTERED_PLUGIN_GROUP  = "KEY_REGISTERED_PLUGIN_GROUP";
const static char *KEY_ENABLED                  = "KEY_ENABLED";
const static char *KEY_TYPE                     = "KEY_TYPE";
const static char *KEY_LIB_FILE                 = "KEY_LIB_FILE";
const static char *KEY_CHECKSUM                 = "KEY_CHECKSUM";
const static char *KEY_PROP_HAS_ABOUT           = "KEY_PROP_HAS_ABOUT";
const static char *KEY_PROP_HAS_SETTINGS        = "KEY_PROP_HAS_SETTINGS";
const static char *KEY_PROP_NAME                = "KEY_PROP_NAME";
const static char *KEY_PROP_DESCRIPTION         = "KEY_PROP_DESCRIPTION";
const static char *KEY_PROP_VERSION             = "KEY_PROP_VERSION";
const static char *KEY_EXTRA_DATA               = "KEY_EXTRA_DATA";

#define GENERATE_KEY(group, key)    QString("%1/%2").arg(group).arg(key)


PluginMgrInternal::PluginMgrInternal(QSharedPointer<PPSettingsInternal> set, QObject *parent)
    : QObject(parent),
    m_setting(set)
{
    const QString path = QString("%1/plugins").arg(QCoreApplication::applicationDirPath());
    if (!m_pluginPaths.contains(path)) {
        m_pluginPaths.append(path);
    }
    reload();
}

PluginMgrInternal::~PluginMgrInternal()
{
    m_setting.data()->internalSettings()->sync();
}

void PluginMgrInternal::addPluginPath(const QString &path)
{
    if (path.isEmpty()) {
        return;
    }
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }
    if (m_pluginPaths.contains(path)) {
        return;
    }
    m_pluginPaths.append(path);

    reload();
}

void PluginMgrInternal::reload()
{
    m_metaList.clear();
    m_dataProviderList.clear();

    foreach (const auto &dir, m_pluginPaths) {
        initPluginByPath(dir);
    }

    foreach (const auto &data, m_metaList) {
        LOG_DEBUG()<<"write "<<data.libraryFile
                 <<" type "<<data.type
                 <<" enable "<<data.enabled;
        write(data);
    }

    initFallbackPlayBackend();
    initFallbackMusicLibraryDAO();

    m_setting.data()->internalSettings()->sync();
}

QList<PluginMetaData> PluginMgrInternal::pluginMetaDataList(BasePlugin::PluginType type) const
{
    if (type == BasePlugin::PluginDataProvider) {
        return m_dataProviderList;
    }
    QList<PluginMetaData> list;
    foreach (const auto &data, m_metaList) {
        if (data.type == type) {
            list.append(data);
        }
    }
    return list;
}

QList<PluginMetaData> PluginMgrInternal::dataProviderList(DataProvider::IDataProvider::SupportedTypes type) const
{
    QList<PluginMetaData> list;
    foreach (const auto &it, m_dataProviderList) {
        if ((it.extraData.toUInt() & type) == type) {
            list.append(it);
        }
    }
    return list;
}

//QList<PluginMetaData> PluginMgrInternal::pluginMetaDataList(DataProvider::IDataProvider::SupportedTypes type) const
//{
//    QList<PluginMetaData> list;
//    foreach (const auto &it, m_dataProviderList) {
//        if ((it.extraData.toUInt() & type) == type) {
//            list.append(it);
//        }
//    }
//    return list;
//}

void PluginMgrInternal::update(const PluginMetaData &data)
{
    int idx = -1;
    bool found = false;
    foreach (const auto &node, m_metaList) {
        idx ++;
        if (node.libraryFile == data.libraryFile) {
            found = true;
            break;
        }
    }
    if (!found) {
        return;
    }
    m_metaList.removeAt(idx);
    m_metaList.append(data);

    if (data.type == BasePlugin::PluginDataProvider) {
        m_dataProviderList.clear();
        foreach (const auto &it, m_metaList) {
            if (it.type == BasePlugin::PluginDataProvider) {
                m_dataProviderList.append(it);
            }
        }
    }
    write(data);
    m_setting.data()->internalSettings()->sync();
}

void PluginMgrInternal::setUsedMusicLibraryDAO(const PluginMetaData &data)
{
    if (m_usedMusicLibraryDAO.libraryFile == data.libraryFile) {
        return;
    }
    m_usedMusicLibraryDAO = data;

    QList<PluginMetaData> list;

    foreach (auto it, m_metaList) {
        if (it.type != BasePlugin::PluginMusicLibraryDAO) {
            list.append(it);
            continue;
        }
        if (it.libraryFile == data.libraryFile) {
            it.enabled = true;
            write(it);
        } else {
            if (it.enabled) {
                it.enabled = false;
                write(it);
            }
        }
        list.append(it);
    }
    m_metaList.clear();
    foreach (const auto  &it, list) {
        m_metaList.append(it);
    }
    m_setting.data()->internalSettings()->sync();
}

void PluginMgrInternal::setUsedPlayBackend(const PluginMetaData &data)
{
    if (m_usedPlayBackend.libraryFile == data.libraryFile) {
        return;
    }
    m_usedPlayBackend = data;

    QList<PluginMetaData> list;

    foreach (auto it, m_metaList) {
        if (it.type != BasePlugin::PluginPlayBackend) {
            list.append(it);
            continue;
        }
        if (it.libraryFile == data.libraryFile) {
            it.enabled = true;
            write(it);
        } else {
            if (it.enabled) {
                it.enabled = false;
                write(it);
            }
        }
        list.append(it);
    }
    m_metaList.clear();
    foreach (const auto  &it, list) {
        m_metaList.append(it);
    }
    m_setting.data()->internalSettings()->sync();
}

void PluginMgrInternal::initPluginByPath(const QString &path)
{
    LOG_DEBUG()<<QString("Search plugin in dir [%1]").arg(path);

    QSettings *innerSettins = m_setting->internalSettings();

    QDir dir(path);
    foreach (const QString &fileName, dir.entryList(QDir::Files)) {
        const QString absFilePath = dir.absoluteFilePath(fileName);
        const QString group = groupName(absFilePath);

        LOG_DEBUG()<<"Try to registe plugin file "<<absFilePath;

        QFileInfo info(absFilePath);
        const QString checksum = QCryptographicHash::hash(info.lastModified().toString().toUtf8()
                                                              + QByteArray::number(info.size())
                                                              + absFilePath.toUtf8(),
                                                          QCryptographicHash::Md5).toHex();
        if (innerSettins->childGroups().contains(group) &&
            (innerSettins->value(GENERATE_KEY(group, KEY_CHECKSUM)).toString() == checksum)) {
                LOG_DEBUG()<<"Plugin file ["<<absFilePath<<"] already registed and no changes.";

                PluginProperty prop;
                prop.hasAbout = innerSettins->value(GENERATE_KEY(group, KEY_PROP_HAS_ABOUT)).toBool();
                prop.hasSettings = innerSettins->value(GENERATE_KEY(group, KEY_PROP_HAS_SETTINGS)).toBool();
                prop.name = innerSettins->value(GENERATE_KEY(group, KEY_PROP_NAME)).toString();
                prop.description = innerSettins->value(GENERATE_KEY(group, KEY_PROP_DESCRIPTION)).toString();
                prop.version = innerSettins->value(GENERATE_KEY(group, KEY_PROP_VERSION)).toString();

                PluginMetaData meta;
                meta.property = prop;
                meta.enabled = innerSettins->value(GENERATE_KEY(group, KEY_ENABLED)).toBool();
                meta.type = (BasePlugin::PluginType)innerSettins->value(GENERATE_KEY(group, KEY_TYPE)).toInt();
                meta.libraryFile = absFilePath;
                meta.checksum = checksum;
                meta.extraData = innerSettins->value(GENERATE_KEY(group, KEY_EXTRA_DATA));
                if (meta.type == BasePlugin::PluginDataProvider) {
                    m_dataProviderList.append(meta);
                }
                m_metaList.append(meta);
                continue;
        }
        QPluginLoader loader(absFilePath);
        QObject *obj = loader.instance();
        if (!obj) {
            LOG_DEBUG()<<"Library file ["<<absFilePath
                      <<"] failed to instance with error ["
                     <<loader.errorString()
                    <<"]";
            continue;
        }
        BasePlugin *base = qobject_cast<BasePlugin*>(obj);
        if (!base) {
            LOG_DEBUG()<<"Library file ["<<absFilePath<<"] failed to case to BasePlugin!";
            loader.unload();
            continue;
        }

        PluginMetaData meta;
        meta.enabled = true;
        meta.type = base->type();
        meta.libraryFile = absFilePath;
        meta.checksum = checksum;
        meta.property = base->property();

        LOG_DEBUG()<<"Plugin "<<meta.libraryFile<<" type "<<meta.type
                 <<" enable "<<meta.enabled;

        if (meta.type == BasePlugin::PluginDataProvider) {
            DataProvider::IDataProvider *dt = qobject_cast<DataProvider::IDataProvider*>(base);
            if (!dt) {
                LOG_WARNING()<<"Plugin type is PluginDataProvider, but can't cast to IDataProvider, ignore !";
                loader.unload();
                continue;
            }
            const int idx = dt->metaObject()->indexOfEnumerator("SupportedTypes");
            if (idx == -1) {
                 LOG_WARNING()<<"Invalid of indexOfEnumerator";
            } else {
                QMetaEnum m = dt->metaObject()->enumerator(idx);
                int hints = 0x0;
                for (int i = 0; i < m.keyCount(); ++i) {
                    int value = m.value(i);
                    LOG_DEBUG()<<"idx "<<i<<" value "<<value<<" name "<<m.valueToKey(value);
                    if (dt->support((DataProvider::IDataProvider::SupportedType)value)) {
                        LOG_DEBUG()<<"suuport "<<m.valueToKey(value);
                        hints |= value;
                    }
                }
                LOG_DEBUG()<<"Final hints "<<hints;
                meta.extraData = hints;
            }
            m_dataProviderList.append(meta);
        } else if (meta.type == BasePlugin::PluginUserInterface) {
            UserInterface::IUserInterface *ui = qobject_cast<UserInterface::IUserInterface*>(base);
            if (!ui) {
                LOG_WARNING()<<"Plugin type is PluginUserInterface, but can't cast to IUserInterface, ignore !";
                loader.unload();
                continue;
            }
            meta.extraData = UserInterface::IUserInterface::convertToExtraData(ui->snapshots());
        }
        m_metaList.append(meta);
        loader.unload();
    }
}

void PluginMgrInternal::write(const PluginMetaData &data)
{
    const QString group = groupName(data.libraryFile);
    QSettings *innerSettins = m_setting->internalSettings();
    innerSettins->setValue(GENERATE_KEY(group, KEY_ENABLED), data.enabled);
    innerSettins->setValue(GENERATE_KEY(group, KEY_TYPE), (int)data.type);
    innerSettins->setValue(GENERATE_KEY(group, KEY_LIB_FILE), data.libraryFile);
    innerSettins->setValue(GENERATE_KEY(group, KEY_CHECKSUM), data.checksum);
    innerSettins->setValue(GENERATE_KEY(group, KEY_PROP_HAS_ABOUT), data.property.hasAbout);
    innerSettins->setValue(GENERATE_KEY(group, KEY_PROP_HAS_SETTINGS), data.property.hasSettings);
    innerSettins->setValue(GENERATE_KEY(group, KEY_PROP_NAME), data.property.name);
    innerSettins->setValue(GENERATE_KEY(group, KEY_PROP_DESCRIPTION), data.property.description);
    innerSettins->setValue(GENERATE_KEY(group, KEY_PROP_VERSION), data.property.version);
    innerSettins->setValue(GENERATE_KEY(group, KEY_EXTRA_DATA), data.extraData);
}

void PluginMgrInternal::initFallbackMusicLibraryDAO()
{
    if (PluginMetaData::isValid(m_usedMusicLibraryDAO)) {
        return;
    }

    QList<PluginMetaData> list;

    foreach (auto it, m_metaList) {
        if (it.type != BasePlugin::PluginMusicLibraryDAO) {
            list.append(it);
            continue;
        }
        if (!PluginMetaData::isValid(it)) {
            continue;
        }
        MusicLibrary::IMusicLibraryDAO *dao =
            qobject_cast<MusicLibrary::IMusicLibraryDAO*>(PluginMgr::instance(it));
        if (!dao) {
            PluginMgr::unload(it);
            continue;
        }
        PluginMgr::unload(it);
        if (PluginMetaData::isValid(m_usedMusicLibraryDAO)) {
            it.enabled = false;
            write(it);
            list.append(it);
            continue;
        }
        m_usedMusicLibraryDAO = it;
        it.enabled = true;
        write(it);
        list.append(it);
    }
    m_metaList.clear();
    foreach (const auto &it, list) {
        m_metaList.append(it);
    }
}

void PluginMgrInternal::initFallbackPlayBackend()
{
    if (PluginMetaData::isValid(m_usedPlayBackend)) {
        LOG_DEBUG()<<" Valid  m_usedPlayBackend "<<m_usedPlayBackend.libraryFile;
        return;
    }

    QList<PluginMetaData> list;

    foreach (auto it, m_metaList) {
        if (it.type != BasePlugin::PluginPlayBackend) {
            list.append(it);
            continue;
        }
        if (!PluginMetaData::isValid(it)) {
            continue;
        }
        PlayBackend::IPlayBackend *dao =
            qobject_cast<PlayBackend::IPlayBackend*>(PluginMgr::instance(it));
        if (!dao) {
            LOG_DEBUG()<<"Can't instance for "<<it.libraryFile;
            PluginMgr::unload(it);
            continue;
        }
        PluginMgr::unload(it);
        if (PluginMetaData::isValid(m_usedPlayBackend)) {
            it.enabled = false;
            write(it);
            list.append(it);
            continue;
        }
        m_usedPlayBackend = it;
        it.enabled = true;
        write(it);
        list.append(it);
    }
    m_metaList.clear();
    foreach (const auto &it, list) {
        m_metaList.append(it);
    }
}

} //PhoenixPlayer
