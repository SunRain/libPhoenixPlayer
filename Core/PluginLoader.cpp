#include <QHash>
#include <QStringList>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>
#include <QMutex>
#include <QDate>

#include "PluginLoader.h"
#include "Backend/IPlayBackend.h"
#include "MusicLibrary/IPlayListDAO.h"
#include "MetadataLookup/IMetadataLookup.h"
#include "MusicLibrary/IMusicTagParser.h"
#include "PluginHost.h"
#include "Decoder/IDecoder.h"
#include "OutPut/IOutPut.h"

namespace PhoenixPlayer {
using namespace MetadataLookup;
using namespace PlayBackend;
using namespace MusicLibrary;
using namespace Decoder;
using namespace OutPut;

#define RETURN_PLUGIN_OBJECT(Class, Type) \
    Class *p = nullptr; \
    if (!m_currentPluginHost[Type]) { \
        foreach (PluginHost *host, m_pluginHostList) { \
            if (host->type () == Type) { \
                QObject *plugin = host->instance (); \
                if (plugin) { \
                    p = qobject_cast<Class *>(plugin); \
                    if (p) { \
                        m_currentPluginHost[Type] = host; \
                        break; \
                    } \
                } \
                host->unLoad (); \
            } \
        } \
    } else { \
        p = qobject_cast<Class *>(m_currentPluginHost[Type]->instance ()); \
    } \
    return p;

PluginLoader::PluginLoader(QObject *parent)
    : QObject(parent)
    ,m_isInit(false)
{
    //TODO: 根据系统来设置插件的默认路径
    QString path = QString("%1/plugins").arg(QCoreApplication::applicationDirPath());

    //初始化一个空容器
    for (int i = (int)Common::PluginTypeAll + 1;
         i < (int)Common::PluginTypeUndefined;
         ++i) {
        m_pluginPath.insert (Common::PluginType(i), path);
        m_currentPluginHost.insert (Common::PluginType(i), 0);
    }
    if (!m_isInit)
        initPlugins ();
}

PluginLoader::~PluginLoader()
{
    qDebug()<<Q_FUNC_INFO;
    if (!m_pluginHostList.isEmpty()) {
        foreach (PluginHost *host, m_pluginHostList) {
            if (host->isLoaded ()) {
                host->unLoad ();
                host->deleteLater ();
                host = 0;
            }
        }
        m_pluginHostList.clear ();
    }

    if (!m_pluginPath.isEmpty ())
        m_pluginPath.clear ();
}

//#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
//PluginLoader *PluginLoader::instance()
//{
//    static QMutex mutex;
//    static QScopedPointer<PluginLoader> scp;

//    if (Q_UNLIKELY(scp.isNull())) {
//        mutex.lock();
//        qDebug()<<Q_FUNC_INFO<<">>>>>>>> statr new";
//        scp.reset(new PluginLoader(0));
//        mutex.unlock();
//    }
//    return scp.data();
//}
//#endif

void PluginLoader::setPluginPath(Common::PluginType type, const QString &path)
{
    if (type == Common::PluginTypeAll) {
        for (int i=(int)Common::PluginTypeAll+1; i<(int)Common::PluginTypeUndefined; ++i) {
            m_pluginPath[Common::PluginType(i)] = path;
        }
    } else {
        m_pluginPath[type] = path;
    }

    initPlugins ();
}

IPlayBackend *PluginLoader::getCurrentPlayBackend()
{
//    IPlayBackend *p = nullptr;
//    if (!mCurrentPluginHost[Common::PluginPlayBackend]) {
//        foreach (PluginHost *host, mPluginHostList) {
//            if (host->type () == Common::PluginPlayBackend) {
//                    QObject *plugin = host->instance ();
//                    if (plugin) {
//                        p = qobject_cast<IPlayBackend *>(plugin);
//                        if (p) {
//                            mCurrentPluginHost[Common::PluginPlayBackend] = host;
//                            break;
//                        }
//                    }
//                    host->unLoad ();
//            }
//        }
//    } else {
//        p = qobject_cast<IPlayBackend *>(mCurrentPluginHost[Common::PluginPlayBackend]->instance ());
//    }
//    return p;
    RETURN_PLUGIN_OBJECT(IPlayBackend, Common::PluginPlayBackend)
}

IPlayListDAO *PluginLoader::getCurrentPlayListDAO()
{
//    IPlayListDAO *p = nullptr;
//    if (!mCurrentPluginHost[Common::PluginPlayListDAO]) {
//        foreach (PluginHost *host, mPluginHostList) {
//            if (host->type () == Common::PluginPlayListDAO) {
//                if (!host->isLoaded ()) {
//                    QObject *plugin = host->instance ();
//                    if (plugin) {
//                        p = qobject_cast<IPlayListDAO *>(plugin);
//                        if (p) {
//                            mCurrentPluginHost[Common::PluginPlayListDAO] = host;
//                            break;
//                        }
//                    }
//                }
//                host->unLoad ();
//            }
//        }
//    } else {
//        p = qobject_cast<IPlayListDAO *>(mCurrentPluginHost[Common::PluginPlayListDAO]->instance ());
//    }
//    return p;
    RETURN_PLUGIN_OBJECT(IPlayListDAO, Common::PluginPlayListDAO)
}

IMusicTagParser *PluginLoader::getCurrentMusicTagParser()
{
//    IMusicTagParser *p = nullptr;
//    if (!mCurrentPluginHost[Common::PluginMusicTagParser]) {
//        foreach (PluginHost *host, mPluginHostList) {
//            if (host->type () == Common::PluginMusicTagParser) {
//                if (!host->isLoaded ()) {
//                    QObject *plugin = host->instance ();
//                    if (plugin) {
//                        p = qobject_cast<IMusicTagParser *>(plugin);
//                        if (p) {
//                            mCurrentPluginHost[Common::PluginMusicTagParser] = host;
//                            break;
//                        }
//                    }
//                }
//                host->unLoad ();
//            }
//        }
//    } else {
//        p = qobject_cast<IMusicTagParser *>(mCurrentPluginHost[Common::PluginMusicTagParser]->instance ());
//    }
//    return p;
    RETURN_PLUGIN_OBJECT(IMusicTagParser, Common::PluginMusicTagParser)
}

IMetadataLookup *PluginLoader::getCurrentMetadataLookup()
{
//    IMetadataLookup *p = nullptr;
//    if (!mCurrentPluginHost[Common::PluginMetadataLookup]) {
//        foreach (PluginHost *host, mPluginHostList) {
//            if (host->type () == Common::PluginMetadataLookup) {
//                if (!host->isLoaded ()) {
//                    QObject *plugin = host->instance ();
//                    if (plugin) {
//                        p = qobject_cast<IMetadataLookup *>(plugin);
//                        if (p) {
//                            mCurrentPluginHost[Common::PluginMetadataLookup] = host;
//                            break;
//                        }
//                    }
//                }
//                host->unLoad ();
//            }
//        }
//    } else {
//        p = qobject_cast<IMetadataLookup *>(mCurrentPluginHost[Common::PluginMetadataLookup]->instance ());
//    }
//    return p;
    RETURN_PLUGIN_OBJECT(IMetadataLookup, Common::PluginMetadataLookup)
}

IDecoder *PluginLoader::getCurrentDecoder()
{
//    IDecoder *p = nullptr;
//    if (!mCurrentPluginHost[Common::PluginDecoder]) {
//        foreach (PluginHost *host, mPluginHostList) {
//            if (host->type () == Common::PluginDecoder) {
//                if (!host->isLoaded ()) {
//                    QObject *plugin = host->instance ();
//                    if (plugin) {
//                        p = qobject_cast<IDecoder *>(plugin);
//                        if (p) {
//                            mCurrentPluginHost[Common::PluginDecoder] = host;
//                            break;
//                        }
//                    }
//                }
//                host->unLoad ();
//            }
//        }
//    } else {
//        p = qobject_cast<IDecoder *>(mCurrentPluginHost[Common::PluginDecoder]->instance ());
//    }
//    return p;
    RETURN_PLUGIN_OBJECT(IDecoder, Common::PluginDecoder)
}

IOutPut *PluginLoader::getCurrentOutPut()
{
//    IOutPut *p = nullptr;
//    if (!mCurrentPluginHost[Common::PluginOutPut]) {
//        foreach (PluginHost *host, mPluginHostList) {
//            if (host->type () == Common::PluginOutPut) {
//                if (!host->isLoaded ()) {
//                    QObject *plugin = host->instance ();
//                    if (plugin) {
//                        p = qobject_cast<IOutPut *>(plugin);
//                        if (p) {
//                            mCurrentPluginHost[Common::PluginOutPut] = host;
//                            break;
//                        }
//                    }
//                }
//                host->unLoad ();
//            }
//        }
//    } else {
//        p = qobject_cast<IOutPut *>(mCurrentPluginHost[Common::PluginOutPut]->instance ());
//    }
//    return p;
    RETURN_PLUGIN_OBJECT(IOutPut, Common::PluginOutPut)
}

QStringList PluginLoader::getPluginHostHashList(Common::PluginType type)
{
    QStringList list;
    if (m_pluginHostList.isEmpty()) {
        qDebug()<<Q_FUNC_INFO<<" Can't find plugins";
        return list;
    }
    switch (type) {
    case Common::PluginMetadataLookup: {
        foreach (PluginHost *obj, m_pluginHostList) {
            if (obj->type () == Common::PluginMetadataLookup)
                list.append(obj->hash ());
        }
        break;
    }
    case Common::PluginMusicTagParser: {
        foreach (PluginHost *obj, m_pluginHostList) {
            if (obj->type () == Common::PluginMusicTagParser)
                list.append(obj->hash ());
        }
        break;
    }
    case Common::PluginPlayBackend: {
        foreach (PluginHost *obj, m_pluginHostList) {
            if (obj->type () == Common::PluginPlayBackend)
                list.append(obj->hash ());
        }
        break;
    }
    case Common::PluginPlayListDAO: {
        foreach (PluginHost *obj, m_pluginHostList) {
            if (obj->type () == Common::PluginPlayListDAO)
                list.append(obj->hash ());
        }
        break;
    }
    case Common::PluginDecoder: {
        foreach (PluginHost *obj, m_pluginHostList) {
            if (obj->type () == Common::PluginDecoder)
                list.append(obj->hash ());
        }
        break;
    }
    case Common::PluginOutPut: {
        foreach (PluginHost *obj, m_pluginHostList) {
            if (obj->type () == Common::PluginOutPut)
                list.append(obj->hash ());
        }
        break;
    }
    case Common::PluginTypeAll: {
        foreach (PluginHost *obj, m_pluginHostList) {
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

int PluginLoader::getPluginHostSize(Common::PluginType type)
{
    if (m_pluginHostList.isEmpty()) {
        qDebug()<<Q_FUNC_INFO<<" Can't find plugins";
        return 0;
    }
    int count = 0;
    if (type == Common::PluginTypeAll) {
        count = m_pluginHostList.size ();
    } else {
        foreach (PluginHost *h, m_pluginHostList) {
            if (h->type () == type)
                count ++;
        }
    }
    return count;
}

PluginHost *PluginLoader::getCurrentPluginHost(Common::PluginType type)
{
    if (type == Common::PluginTypeUndefined || type == Common::PluginTypeAll)
        return 0;
    return m_currentPluginHost[type];
}

PluginHost *PluginLoader::getPluginHostByHash(const QString &hostHash)
{
    if (hostHash.isEmpty ())
        return nullptr;
    foreach (PluginHost *h, m_pluginHostList) {
        if (h->hash () == hostHash)
            return h;
    }
    return nullptr;
}

QList<PluginHost *> PluginLoader::getPluginHostList(Common::PluginType type)
{
    if (type == Common::PluginTypeAll)
        return m_pluginHostList;
    QList<PluginHost *> list;
    if (m_pluginHostList.isEmpty () || type == Common::PluginTypeUndefined)
        return list;
    foreach (PluginHost *host, m_pluginHostList) {
        if (host->type () == type) {
            list.append (host);
        }
    }
    qDebug()<<Q_FUNC_INFO<<QString("Host num of type [%1] is [%2]").arg (type).arg (list.size ());
    return list;
}

void PluginLoader::initPlugins()
{
    //删除之前容器中保存的插件
    if (!m_pluginHostList.isEmpty()) {
        foreach (PluginHost *host, m_pluginHostList) {
            if (host->isLoaded ()) {
                host->unLoad ();
                host->deleteLater ();
                host = 0;
            }
            m_pluginHostList.clear ();
        }
    }
    QStringList existPaths;
    for (int i = (int)Common::PluginTypeAll + 1; i < (int)Common::PluginTypeUndefined; ++i) {
        // dynamic plugins
        qDebug()<<Q_FUNC_INFO<<"Search plugin in dir ["<<m_pluginPath[Common::PluginType(i)] <<"]";

        QDir dir(m_pluginPath[Common::PluginType(i)]);
        foreach (QString fileName, dir.entryList(QDir::Files)) {
            QString absFilePath = dir.absoluteFilePath(fileName);

            if (existPaths.contains (absFilePath))
                continue;
            existPaths.append (absFilePath);

            PluginHost *host = new PluginHost(absFilePath, this);
            if (host->isValid ()) {
                m_pluginHostList.append (host);
            } else {
                delete host;
                host = 0;
            }
        }
    }
    qDebug()<<Q_FUNC_INFO<<" find plugin num "<<m_pluginHostList.size ();
    foreach (PluginHost *obj, m_pluginHostList) {
        qDebug()<<"Found plugins for type ["
               <<obj->type ()<<"] name ["
              <<obj->name ()<<"] file ["
             <<obj->libraryFile ()<<"]";
    }

    m_isInit = true;
}

void PluginLoader::setNewPlugin(Common::PluginType type, const QString &newPluginHash)
{
    if (m_pluginHostList.isEmpty()) {
        qDebug()<<Q_FUNC_INFO<<" mPluginList is empty!!!!";
        return;
    }
    bool changed = false;

    if (m_currentPluginHost[type] && m_currentPluginHost[type]->hash () == newPluginHash)
        return;
    foreach (PluginHost *host, m_pluginHostList) {
        if (host->type () == type && host->hash () == newPluginHash) {
            m_currentPluginHost[type] = host;
            changed = true;
            break;
        }
    }
    if (changed)
        emit signalPluginChanged (type);
}
}
