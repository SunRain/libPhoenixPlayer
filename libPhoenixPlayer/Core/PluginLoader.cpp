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
#include "Backend/BackendHost.h"
#include "MetadataLookup/IMetadataLookup.h"
#include "MusicLibrary/IMusicTagParser.h"
#include "MusicLibrary/MusicLibraryDAOHost.h"
#include "PluginHost.h"
//#include "Decoder/IDecoder.h"
//#include "OutPut/IOutPut.h"
//#include "OutPut/OutPutHost.h"
#include "PPSettings.h"

namespace PhoenixPlayer {
using namespace MetadataLookup;
using namespace PlayBackend;
using namespace MusicLibrary;
//using namespace Decoder;
//using namespace OutPut;

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

PluginLoader::PluginLoader(PPSettings *set, QObject *parent)
    : QObject(parent)
    , m_settings(set)
{
//    m_settings = Settings::instance ();
//    m_settings = phoenixPlayerLib->settings ();
    m_curBackendHost = nullptr;
    m_curDAOHost = nullptr;
//    m_curOutPutHost = nullptr;
    //TODO: 根据系统来设置插件的默认路径
    QString path = QString("%1/plugins").arg(QCoreApplication::applicationDirPath());
    initPluginByPath (path);
}

PluginLoader::~PluginLoader()
{
    qDebug()<<Q_FUNC_INFO;
    if (m_curBackendHost && m_curBackendHost->isLoaded ()) {
        if (!m_curBackendHost->unLoad ())
            m_curBackendHost->forceUnload ();
        m_curBackendHost->deleteLater ();
        m_curBackendHost = nullptr;
    }
    if (m_curDAOHost && m_curDAOHost->isLoaded ()) {
        if (!m_curDAOHost->unLoad ())
            m_curDAOHost->forceUnload ();
        m_curDAOHost->deleteLater ();
        m_curDAOHost = nullptr;
    }
//    if (m_curOutPutHost && m_curOutPutHost) {
//        if (!m_curOutPutHost->unLoad ())
//            m_curOutPutHost->forceUnload ();
//        m_curOutPutHost->deleteLater ();
//        m_curOutPutHost = nullptr;
//    }
}

void PluginLoader::addPluginPath(const QString &path)
{
    initPluginByPath (path);
}

BackendHost *PluginLoader::curBackendHost()
{
    if (!m_curBackendHost) {
        QString lib = m_settings->curPlayBackend ();
        m_curBackendHost = new BackendHost(lib, this);
        if (!m_curBackendHost->isValid ()) {
            if (m_curBackendHost) {
                m_curBackendHost->deleteLater ();
                m_curBackendHost = nullptr;
            }
            QStringList list = m_libraries.values (PPCommon::PluginPlayBackend);
            if (list.isEmpty ()) {
                m_curBackendHost = nullptr;
            } else {
                foreach (QString str, list) {
                    m_curBackendHost = new BackendHost(str, this);
                    if (m_curBackendHost->isValid ())
                        break;
                    m_curBackendHost->deleteLater ();
                    m_curBackendHost = nullptr;
                }
            }
        }
    }
    return m_curBackendHost;
}

//OutPutHost *PluginLoader::curOutPutHost()
//{
//    if (!m_curOutPutHost) {
//        QString lib = m_settings->curOutPut ();
//        m_curOutPutHost = new OutPutHost(lib, this);
//        if (!m_curOutPutHost->isValid ()) {
//            if (m_curOutPutHost) {
//                m_curOutPutHost->deleteLater ();
//                m_curOutPutHost = nullptr;
//            }
//            QStringList list = m_libraries.values (PPCommon::PluginOutPut);
//            if (list.isEmpty ()) {
//                m_curOutPutHost = nullptr;
//            } else {
//                foreach (QString str, list) {
//                    m_curOutPutHost = new OutPutHost(str, this);
//                    if (m_curOutPutHost->isValid ())
//                        break;
//                    m_curOutPutHost->deleteLater ();
//                    m_curOutPutHost = nullptr;
//                }
//            }
//        }
//    }
//    return m_curOutPutHost;
//}

MusicLibraryDAOHost *PluginLoader::curDAOHost()
{
    if (!m_curDAOHost) {
        QString lib = m_settings->curMusicLibraryDAO ();
        m_curDAOHost = new MusicLibraryDAOHost(lib, this);
        if (!m_curDAOHost->isValid ()) {
            if (m_curDAOHost) {
                m_curDAOHost->deleteLater ();
                m_curDAOHost = nullptr;
            }
            QStringList list = m_libraries.values (PPCommon::PluginMusicLibraryDAO);
            if (list.isEmpty ()) {
                m_curDAOHost = nullptr;
            } else {
                foreach (QString str, list) {
                    m_curDAOHost = new MusicLibraryDAOHost(str, this);
                    if (m_curDAOHost->isValid ())
                        break;
                    m_curDAOHost->deleteLater ();
                    m_curDAOHost = nullptr;
                }
            }
        }
    }
    return m_curDAOHost;
}

QStringList PluginLoader::pluginLibraries(PPCommon::PluginType type)
{
    if (type == PPCommon::PluginTypeAll)
        return m_libraries.values ();
    return m_libraries.values(type);
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

//void PluginLoader::setPluginPath(Common::PluginType type, const QString &path)
//{
//    if (type == Common::PluginTypeAll) {
//        for (int i=(int)Common::PluginTypeAll+1; i<(int)Common::PluginTypeUndefined; ++i) {
//            m_pluginPath[Common::PluginType(i)] = path;
//        }
//    } else {
//        m_pluginPath[type] = path;
//    }

//    initPlugins ();
//}

//IPlayBackend *PluginLoader::getCurrentPlayBackend()
//{
////    IPlayBackend *p = nullptr;
////    if (!mCurrentPluginHost[Common::PluginPlayBackend]) {
////        foreach (PluginHost *host, mPluginHostList) {
////            if (host->type () == Common::PluginPlayBackend) {
////                    QObject *plugin = host->instance ();
////                    if (plugin) {
////                        p = qobject_cast<IPlayBackend *>(plugin);
////                        if (p) {
////                            mCurrentPluginHost[Common::PluginPlayBackend] = host;
////                            break;
////                        }
////                    }
////                    host->unLoad ();
////            }
////        }
////    } else {
////        p = qobject_cast<IPlayBackend *>(mCurrentPluginHost[Common::PluginPlayBackend]->instance ());
////    }
////    return p;
//    RETURN_PLUGIN_OBJECT(IPlayBackend, Common::PluginPlayBackend)
//}

//IMusicLibraryDAO *PluginLoader::getCurrentLibraryDAO()
//{
////    IPlayListDAO *p = nullptr;
////    if (!mCurrentPluginHost[Common::PluginPlayListDAO]) {
////        foreach (PluginHost *host, mPluginHostList) {
////            if (host->type () == Common::PluginPlayListDAO) {
////                if (!host->isLoaded ()) {
////                    QObject *plugin = host->instance ();
////                    if (plugin) {
////                        p = qobject_cast<IPlayListDAO *>(plugin);
////                        if (p) {
////                            mCurrentPluginHost[Common::PluginPlayListDAO] = host;
////                            break;
////                        }
////                    }
////                }
////                host->unLoad ();
////            }
////        }
////    } else {
////        p = qobject_cast<IPlayListDAO *>(mCurrentPluginHost[Common::PluginPlayListDAO]->instance ());
////    }
////    return p;
//    RETURN_PLUGIN_OBJECT(IMusicLibraryDAO, Common::PluginMusicLibraryDAO)
//}

//IMusicTagParser *PluginLoader::getCurrentMusicTagParser()
//{
////    IMusicTagParser *p = nullptr;
////    if (!mCurrentPluginHost[Common::PluginMusicTagParser]) {
////        foreach (PluginHost *host, mPluginHostList) {
////            if (host->type () == Common::PluginMusicTagParser) {
////                if (!host->isLoaded ()) {
////                    QObject *plugin = host->instance ();
////                    if (plugin) {
////                        p = qobject_cast<IMusicTagParser *>(plugin);
////                        if (p) {
////                            mCurrentPluginHost[Common::PluginMusicTagParser] = host;
////                            break;
////                        }
////                    }
////                }
////                host->unLoad ();
////            }
////        }
////    } else {
////        p = qobject_cast<IMusicTagParser *>(mCurrentPluginHost[Common::PluginMusicTagParser]->instance ());
////    }
////    return p;
//    RETURN_PLUGIN_OBJECT(IMusicTagParser, Common::PluginMusicTagParser)
//}

//IMetadataLookup *PluginLoader::getCurrentMetadataLookup()
//{
////    IMetadataLookup *p = nullptr;
////    if (!mCurrentPluginHost[Common::PluginMetadataLookup]) {
////        foreach (PluginHost *host, mPluginHostList) {
////            if (host->type () == Common::PluginMetadataLookup) {
////                if (!host->isLoaded ()) {
////                    QObject *plugin = host->instance ();
////                    if (plugin) {
////                        p = qobject_cast<IMetadataLookup *>(plugin);
////                        if (p) {
////                            mCurrentPluginHost[Common::PluginMetadataLookup] = host;
////                            break;
////                        }
////                    }
////                }
////                host->unLoad ();
////            }
////        }
////    } else {
////        p = qobject_cast<IMetadataLookup *>(mCurrentPluginHost[Common::PluginMetadataLookup]->instance ());
////    }
////    return p;
//    RETURN_PLUGIN_OBJECT(IMetadataLookup, Common::PluginMetadataLookup)
//}

//IDecoder *PluginLoader::getCurrentDecoder()
//{
////    IDecoder *p = nullptr;
////    if (!mCurrentPluginHost[Common::PluginDecoder]) {
////        foreach (PluginHost *host, mPluginHostList) {
////            if (host->type () == Common::PluginDecoder) {
////                if (!host->isLoaded ()) {
////                    QObject *plugin = host->instance ();
////                    if (plugin) {
////                        p = qobject_cast<IDecoder *>(plugin);
////                        if (p) {
////                            mCurrentPluginHost[Common::PluginDecoder] = host;
////                            break;
////                        }
////                    }
////                }
////                host->unLoad ();
////            }
////        }
////    } else {
////        p = qobject_cast<IDecoder *>(mCurrentPluginHost[Common::PluginDecoder]->instance ());
////    }
////    return p;
//    RETURN_PLUGIN_OBJECT(IDecoder, Common::PluginDecoder)
//}

//IOutPut *PluginLoader::getCurrentOutPut()
//{
////    IOutPut *p = nullptr;
////    if (!mCurrentPluginHost[Common::PluginOutPut]) {
////        foreach (PluginHost *host, mPluginHostList) {
////            if (host->type () == Common::PluginOutPut) {
////                if (!host->isLoaded ()) {
////                    QObject *plugin = host->instance ();
////                    if (plugin) {
////                        p = qobject_cast<IOutPut *>(plugin);
////                        if (p) {
////                            mCurrentPluginHost[Common::PluginOutPut] = host;
////                            break;
////                        }
////                    }
////                }
////                host->unLoad ();
////            }
////        }
////    } else {
////        p = qobject_cast<IOutPut *>(mCurrentPluginHost[Common::PluginOutPut]->instance ());
////    }
////    return p;
//    RETURN_PLUGIN_OBJECT(IOutPut, Common::PluginOutPut)
//}

//QStringList PluginLoader::getPluginHostHashList(Common::PluginType type)
//{
//    QStringList list;
//    if (m_pluginHostList.isEmpty()) {
//        qDebug()<<Q_FUNC_INFO<<" Can't find plugins";
//        return list;
//    }
//    switch (type) {
//    case Common::PluginMetadataLookup: {
//        foreach (PluginHost *obj, m_pluginHostList) {
//            if (obj->type () == Common::PluginMetadataLookup)
//                list.append(obj->hash ());
//        }
//        break;
//    }
//    case Common::PluginMusicTagParser: {
//        foreach (PluginHost *obj, m_pluginHostList) {
//            if (obj->type () == Common::PluginMusicTagParser)
//                list.append(obj->hash ());
//        }
//        break;
//    }
//    case Common::PluginPlayBackend: {
//        foreach (PluginHost *obj, m_pluginHostList) {
//            if (obj->type () == Common::PluginPlayBackend)
//                list.append(obj->hash ());
//        }
//        break;
//    }
//    case Common::PluginMusicLibraryDAO: {
//        foreach (PluginHost *obj, m_pluginHostList) {
//            if (obj->type () == Common::PluginMusicLibraryDAO)
//                list.append(obj->hash ());
//        }
//        break;
//    }
//    case Common::PluginDecoder: {
//        foreach (PluginHost *obj, m_pluginHostList) {
//            if (obj->type () == Common::PluginDecoder)
//                list.append(obj->hash ());
//        }
//        break;
//    }
//    case Common::PluginOutPut: {
//        foreach (PluginHost *obj, m_pluginHostList) {
//            if (obj->type () == Common::PluginOutPut)
//                list.append(obj->hash ());
//        }
//        break;
//    }
//    case Common::PluginTypeAll: {
//        foreach (PluginHost *obj, m_pluginHostList) {
//            list.append (obj->hash ());
//        }
//        break;
//    }
//    default:
//        qDebug()<<"Invalid plugin type.";
//        break;
//    }
//    return list;
//}

//int PluginLoader::getPluginHostSize(Common::PluginType type)
//{
//    if (m_pluginHostList.isEmpty()) {
//        qDebug()<<Q_FUNC_INFO<<" Can't find plugins";
//        return 0;
//    }
//    int count = 0;
//    if (type == Common::PluginTypeAll) {
//        count = m_pluginHostList.size ();
//    } else {
//        foreach (PluginHost *h, m_pluginHostList) {
//            if (h->type () == type)
//                count ++;
//        }
//    }
//    return count;
//}

//PluginHost *PluginLoader::getCurrentPluginHost(Common::PluginType type)
//{
//    if (type == Common::PluginTypeUndefined || type == Common::PluginTypeAll)
//        return 0;
//    return m_currentPluginHost[type];
//}

//PluginHost *PluginLoader::getPluginHostByHash(const QString &hostHash)
//{
//    if (hostHash.isEmpty ())
//        return nullptr;
//    foreach (PluginHost *h, m_pluginHostList) {
//        if (h->hash () == hostHash)
//            return h;
//    }
//    return nullptr;
//}

//QList<PluginHost *> PluginLoader::getPluginHostList(Common::PluginType type)
//{
//    if (type == Common::PluginTypeAll)
//        return m_pluginHostList;
//    QList<PluginHost *> list;
//    if (m_pluginHostList.isEmpty () || type == Common::PluginTypeUndefined)
//        return list;
//    foreach (PluginHost *host, m_pluginHostList) {
//        if (host->type () == type) {
//            list.append (host);
//        }
//    }
//    qDebug()<<Q_FUNC_INFO<<QString("Host num of type [%1] is [%2]").arg (type).arg (list.size ());
//    return list;
//}

void PluginLoader::initPluginByPath(const QString &path)
{
    qDebug()<<Q_FUNC_INFO<<QString("Search plugin in dir [%1]").arg (path);

    QDir dir(path);
    foreach (const QString &fileName, dir.entryList(QDir::Files)) {
        QString absFilePath = dir.absoluteFilePath(fileName);
        QPluginLoader loader(absFilePath, this);
        QJsonObject obj = loader.metaData ();
        if (!obj.isEmpty ()) {
            QString iid = obj.value ("IID").toString ();
            if (iid.startsWith ("PhoenixPlayer.PlayBackend")) {
                m_libraries.insertMulti (PPCommon::PluginPlayBackend, absFilePath);
            } else if (iid.startsWith ("PhoenixPlayer.MetadataLookup")) {
                m_libraries.insertMulti (PPCommon::PluginMetadataLookup, absFilePath);
            } else if (iid.startsWith ("PhoenixPlayer.MusicTagParser")) {
                m_libraries.insertMulti (PPCommon::PluginMusicTagParser, absFilePath);
            } else if (iid.startsWith ("PhoenixPlayer.MusicLibraryDAO")) {
                m_libraries.insertMulti (PPCommon::PluginMusicLibraryDAO, absFilePath);
            } else if (iid.startsWith ("PhoenixPlayer.Decoder")) {
                m_libraries.insertMulti (PPCommon::PluginDecoder, absFilePath);
            } else if (iid.startsWith ("PhoenixPlayer.OutPut")) {
                m_libraries.insertMulti (PPCommon::PluginOutPut, absFilePath);
            } else if (iid.startsWith("PhoenixPlayer.SpectrumGenerator")) {
                m_libraries.insertMulti(PPCommon::PluginSpectrumGenerator, absFilePath);
            } else {
                m_libraries.insertMulti (PPCommon::PluginTypeUndefined, absFilePath);
            }
        }
    }
    qDebug()<<Q_FUNC_INFO<<" find plugin num "<<m_libraries.size ();
//    foreach (PluginHost *obj, m_pluginHostList) {
//        qDebug()<<"Found plugins for type ["
//               <<obj->type ()<<"] name ["
//              <<obj->name ()<<"] file ["
//             <<obj->libraryFile ()<<"]";
//    }
    foreach (PPCommon::PluginType t, m_libraries.uniqueKeys ()) {
        foreach (const QString &str, m_libraries.values (t)) {
            qDebug()<<Q_FUNC_INFO<<QString("Found plugin for type [%1], file [%2]").arg (t).arg (str);
        }
    }
}

//void PluginLoader::setNewPlugin(Common::PluginType type, const QString &newPluginHash)
//{
//    if (m_pluginHostList.isEmpty()) {
//        qDebug()<<Q_FUNC_INFO<<" mPluginList is empty!!!!";
//        return;
//    }
//    bool changed = false;

//    if (m_currentPluginHost[type] && m_currentPluginHost[type]->hash () == newPluginHash)
//        return;
//    foreach (PluginHost *host, m_pluginHostList) {
//        if (host->type () == type && host->hash () == newPluginHash) {
//            m_currentPluginHost[type] = host;
//            changed = true;
//            break;
//        }
//    }
//    if (changed)
//        emit signalPluginChanged (type);
//}
}
