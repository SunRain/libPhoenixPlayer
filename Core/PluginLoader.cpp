#include <QHash>
#include <QStringList>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>
#include <QMutex>

#include "PluginLoader.h"
#include "Backend/IPlayBackend.h"
#include "MusicLibrary/IPlayListDAO.h"
#include "MetadataLookup/IMetadataLookup.h"
#include "MusicLibrary/IMusicTagParser.h"
#include "PluginHost.h"

namespace PhoenixPlayer {
using namespace MetadataLookup;
using namespace PlayBackend;
using namespace MusicLibrary;

PluginLoader::PluginLoader(QObject *parent)
    : QObject(parent)
    ,isInit(false)
{
    //TODO: 根据系统来设置插件的默认路径
    QString path = QString("%1/plugins").arg(QCoreApplication::applicationDirPath());

    //初始化一个空容器
    for (int i = (int)Common::PluginTypeAll + 1;
         i < (int)Common::PluginTypeUndefined;
         ++i) {
        mPluginPath.insert (Common::PluginType(i), path);
        mCurrentPluginHost.insert (Common::PluginType(i), 0);
    }
    if (!isInit)
        initPlugins ();
}

PluginLoader::~PluginLoader()
{
    qDebug()<<Q_FUNC_INFO;
    if (!mPluginHostList.isEmpty()) {
        foreach (PluginHost *host, mPluginHostList) {
            if (host->isLoaded ()) {
                host->unLoad ();
                host->deleteLater ();
                host = 0;
            }
        }
        mPluginHostList.clear ();
    }

    if (!mPluginPath.isEmpty ())
        mPluginPath.clear ();
}

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
PluginLoader *PluginLoader::instance()
{
    static QMutex mutex;
    static QScopedPointer<PluginLoader> scp;

    if (Q_UNLIKELY(scp.isNull())) {
        mutex.lock();
        qDebug()<<Q_FUNC_INFO<<">>>>>>>> statr new";
        scp.reset(new PluginLoader(0));
        mutex.unlock();
    }
    return scp.data();
}
#endif

void PluginLoader::setPluginPath(Common::PluginType type, const QString &path)
{
    if (type == Common::PluginTypeAll) {
        for (int i=(int)Common::PluginTypeAll+1; i<(int)Common::PluginTypeUndefined; ++i) {
            mPluginPath[Common::PluginType(i)] = path;
        }
    } else {
        mPluginPath[type] = path;
    }

    initPlugins ();
}

IPlayBackend *PluginLoader::getCurrentPlayBackend()
{
    IPlayBackend *p = nullptr;
    if (!mCurrentPluginHost[Common::PluginPlayBackend]) {
        foreach (PluginHost *host, mPluginHostList) {
            if (host->type () == Common::PluginPlayBackend) {
                if (!host->isLoaded ()) {
                    QObject *plugin = host->instance ();
                    if (plugin) {
                        p = qobject_cast<IPlayBackend *>(plugin);
                        if (p) {
                            mCurrentPluginHost[Common::PluginPlayBackend] = host;
                            break;
                        }
                    }
                }
                host->unLoad ();
            }
        }
    } else {
        p = qobject_cast<IPlayBackend *>(mCurrentPluginHost[Common::PluginPlayBackend]->instance ());
    }
    return p;
}

IPlayListDAO *PluginLoader::getCurrentPlayListDAO()
{
    IPlayListDAO *p = nullptr;
    if (!mCurrentPluginHost[Common::PluginPlayListDAO]) {
        foreach (PluginHost *host, mPluginHostList) {
            if (host->type () == Common::PluginPlayListDAO) {
                if (!host->isLoaded ()) {
                    QObject *plugin = host->instance ();
                    if (plugin) {
                        p = qobject_cast<IPlayListDAO *>(plugin);
                        if (p) {
                            mCurrentPluginHost[Common::PluginPlayListDAO] = host;
                            break;
                        }
                    }
                }
                host->unLoad ();
            }
        }
    } else {
        p = qobject_cast<IPlayListDAO *>(mCurrentPluginHost[Common::PluginPlayListDAO]->instance ());
    }
    return p;
}

IMusicTagParser *PluginLoader::getCurrentMusicTagParser()
{
    IMusicTagParser *p = nullptr;
    if (!mCurrentPluginHost[Common::PluginMusicTagParser]) {
        foreach (PluginHost *host, mPluginHostList) {
            if (host->type () == Common::PluginMusicTagParser) {
                if (!host->isLoaded ()) {
                    QObject *plugin = host->instance ();
                    if (plugin) {
                        p = qobject_cast<IMusicTagParser *>(plugin);
                        if (p) {
                            mCurrentPluginHost[Common::PluginMusicTagParser] = host;
                            break;
                        }
                    }
                }
                host->unLoad ();
            }
        }
    } else {
        p = qobject_cast<IMusicTagParser *>(mCurrentPluginHost[Common::PluginMusicTagParser]->instance ());
    }
    return p;
}

IMetadataLookup *PluginLoader::getCurrentMetadataLookup()
{
    IMetadataLookup *p = nullptr;
    if (!mCurrentPluginHost[Common::PluginMetadataLookup]) {
        foreach (PluginHost *host, mPluginHostList) {
            if (host->type () == Common::PluginMetadataLookup) {
                if (!host->isLoaded ()) {
                    QObject *plugin = host->instance ();
                    if (plugin) {
                        p = qobject_cast<IMetadataLookup *>(plugin);
                        if (p) {
                            mCurrentPluginHost[Common::PluginMetadataLookup] = host;
                            break;
                        }
                    }
                }
                host->unLoad ();
            }
        }
    } else {
        p = qobject_cast<IMetadataLookup *>(mCurrentPluginHost[Common::PluginMetadataLookup]->instance ());
    }
    return p;
}

QStringList PluginLoader::getPluginHostHashList(Common::PluginType type)
{
    QStringList list;
    if (mPluginHostList.isEmpty()) {
        qDebug()<<Q_FUNC_INFO<<" Can't find plugins";
        return list;
    }
    switch (type) {
    case Common::PluginMetadataLookup: {
        foreach (PluginHost *obj, mPluginHostList) {
            if (obj->type () == Common::PluginMetadataLookup)
                list.append(obj->hash ());
        }
        break;
    }
    case Common::PluginMusicTagParser: {
        foreach (PluginHost *obj, mPluginHostList) {
            if (obj->type () == Common::PluginMusicTagParser)
                list.append(obj->hash ());
        }
        break;
    }
    case Common::PluginPlayBackend: {
        foreach (PluginHost *obj, mPluginHostList) {
            if (obj->type () == Common::PluginPlayBackend)
                list.append(obj->hash ());
        }
        break;
    }
    case Common::PluginPlayListDAO: {
        foreach (PluginHost *obj, mPluginHostList) {
            if (obj->type () == Common::PluginPlayListDAO)
                list.append(obj->hash ());
        }
        break;
    }
    case Common::PluginTypeAll: {
        foreach (PluginHost *obj, mPluginHostList) {
            list.append (obj->hash ());
        }
        break;
    }
    default:
        qDebug()<<"Invalid plugin type.";
        break;
    }
    return list;
}

PluginHost *PluginLoader::getCurrentPluginHost(Common::PluginType type)
{
    if (type == Common::PluginTypeUndefined || type == Common::PluginTypeAll)
        return 0;
    return mCurrentPluginHost[type];
}

QList<PluginHost *> PluginLoader::getPluginHostList(Common::PluginType type)
{
    QList<PluginHost *> list;
    if (mPluginHostList.isEmpty ())
        return list;
    foreach (PluginHost *host, mPluginHostList) {
        if (host->type () == type) {
            list.append (host);
        }
    }
    return list;
}

void PluginLoader::initPlugins()
{
    //删除之前容器中保存的插件
    if (!mPluginHostList.isEmpty()) {
        foreach (PluginHost *host, mPluginHostList) {
            if (host->isLoaded ()) {
                host->unLoad ();
                host->deleteLater ();
                host = 0;
            }
            mPluginHostList.clear ();
        }
    }
    QStringList existPaths;
    for (int i = (int)Common::PluginTypeAll + 1; i < (int)Common::PluginTypeUndefined; ++i) {
        // dynamic plugins
        qDebug()<<Q_FUNC_INFO<<"Search plugin in dir ["<<mPluginPath[Common::PluginType(i)] <<"]";

        QDir dir(mPluginPath[Common::PluginType(i)]);
        foreach (QString fileName, dir.entryList(QDir::Files)) {
            QString absFilePath = dir.absoluteFilePath(fileName);

            if (existPaths.contains (absFilePath))
                continue;
            existPaths.append (absFilePath);

            PluginHost *host = new PluginHost(absFilePath, this);
            if (host->isValid ()) {
                mPluginHostList.append (host);
            } else {
                delete host;
                host = 0;
            }
        }
    }
    qDebug()<<Q_FUNC_INFO<<" find plugin num "<<mPluginHostList.size ();
    foreach (PluginHost *obj, mPluginHostList) {
        qDebug()<<"Found plugins for type ["
               <<obj->type ()<<"] name ["
              <<obj->name ()<<"] file ["
             <<obj->libraryFile ()<<"]";
    }

    isInit = true;
}

void PluginLoader::setNewPlugin(Common::PluginType type, const QString &newPluginHash)
{
    if (mPluginHostList.isEmpty()) {
        qDebug()<<Q_FUNC_INFO<<" mPluginList is empty!!!!";
        return;
    }
    bool changed = false;

    if (mCurrentPluginHost[type] && mCurrentPluginHost[type]->hash () == newPluginHash)
        return;
    foreach (PluginHost *host, mPluginHostList) {
        if (host->type () == type && host->hash () == newPluginHash) {
            mCurrentPluginHost[type] = host;
            changed = true;
            break;
        }
    }
    if (changed)
        emit signalPluginChanged (type);
}
}
