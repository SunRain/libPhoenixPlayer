#include "PluginHost.h"

#include <QPluginLoader>
#include <QJsonObject>
#include <QDebug>
#include <QFileInfo>

#include "Util.h"

namespace PhoenixPlayer {

//PluginHost::PluginHost(const QString &libraryFile, QObject *parent)
//    : QObject(parent),
//      m_libraryFile(libraryFile),
//      m_pluginObject(0)
//{
//    m_valid = false;
//    m_pluginType = Common::PluginType::PluginTypeUndefined;
//    m_pluginLoader = new QPluginLoader(libraryFile, this);
//    QFileInfo f(libraryFile);
//    QString libraryPath = f.absolutePath ();

//    QJsonObject obj = m_pluginLoader->metaData ();
//    if (!obj.isEmpty ()) {
//        QString iid = obj.value ("IID").toString ();
//        m_valid = true;
//        if (iid.startsWith ("PhoenixPlayer.PlayBackend")) {
//            m_pluginType = Common::PluginPlayBackend;
//        } else if (iid.startsWith ("PhoenixPlayer.MetadataLookup")) {
//            m_pluginType = Common::PluginMetadataLookup;
//        } else if (iid.startsWith ("PhoenixPlayer.MusicTagParser")) {
//            m_pluginType = Common::PluginMusicTagParser;
//        } else if (iid.startsWith ("PhoenixPlayer.MusicLibraryDAO")) {
//            m_pluginType = Common::PluginMusicLibraryDAO;
//        } else if (iid.startsWith ("PhoenixPlayer.Decoder")) {
//            m_pluginType = Common::PluginDecoder;
//        } else if (iid.startsWith ("PhoenixPlayer.OutPut")) {
//            m_pluginType = Common::PluginOutPut;
//        } else {
//            m_pluginType = Common::PluginTypeUndefined;
//            m_valid = false;
//        }
//        QJsonObject md = obj.value ("MetaData").toObject ();
//        if (!md.isEmpty ()) {
//            m_name = md.value ("name").toString ();
//            m_version = md.value ("version").toString ();
//            m_description = md.value ("description").toString ();
//            m_hash = Util::calculateHash (libraryFile);
//            m_configFile = md.value ("configFile").toString ();
//            if (!m_configFile.isEmpty ()) {
//                m_configFile = QString("%1/%2")
//                        .arg (libraryPath).arg (m_configFile);
//            }
//        } else {
//            m_valid = false;
//        }
//    } else {
//        qDebug()<<Q_FUNC_INFO<<" get metadata error for plugin ["<<m_libraryFile
//               <<"] error is ["<<m_pluginLoader->errorString ()<<"]";
//    }
//    if (m_pluginLoader->isLoaded ()) {
//        if (!m_pluginLoader->unload ())
//            qDebug()<<Q_FUNC_INFO<<" fail to unload plugin due to "<<m_pluginLoader->errorString ();
//    }
//    delete m_pluginLoader;
//    m_pluginLoader = 0;
//}

PluginHost::PluginHost(const QString &libraryFile, QObject *parent)
    :QObject(parent)
{
    m_pluginObject = nullptr;
    m_libraryFile = libraryFile;
    m_pluginLoader = new QPluginLoader(libraryFile, this);

    m_valid = false;
    QJsonObject obj = m_pluginLoader->metaData ();
    if (!obj.isEmpty ()) {
        m_metaObject = obj.value ("MetaData").toObject ();
        if (!m_metaObject.isEmpty ()) {
            m_name = m_metaObject.value ("name").toString ();
            m_version = m_metaObject.value ("version").toString ();
            m_description = m_metaObject.value ("description").toString ();
//            m_hash = Util::calculateHash (libraryFile);
            m_configFile = m_metaObject.value ("configFile").toString ();
            m_valid = true;
        }
    }
    delete m_pluginLoader;
    m_pluginLoader = nullptr;
}

PluginHost::PluginHost(const QString &libraryFile, const QJsonObject &metaData, QObject *parent)
    : QObject(parent)
{
    m_pluginLoader = nullptr;
    m_pluginObject = nullptr;
    m_libraryFile = libraryFile;
    m_valid = false;

    m_metaObject = metaData;
    if (!metaData.isEmpty ()) {
        m_name = metaData.value ("name").toString ();
        m_version = metaData.value ("version").toString ();
        m_description = metaData.value ("description").toString ();
//        m_hash = Util::calculateHash (libraryFile);
        m_configFile = metaData.value ("configFile").toString ();
        m_valid = true;
    }
}

PluginHost::~PluginHost()
{
    if (m_pluginLoader) {
        if (m_pluginLoader->isLoaded ())
            m_pluginLoader->unload ();
        delete m_pluginLoader;
        m_pluginLoader = nullptr;
    }
    if (m_pluginObject) {
        delete m_pluginObject;
        m_pluginObject = nullptr;
    }
}

//Common::PluginType PluginHost::type() const
//{
////    return m_pluginType;
//    return Common::PluginTypeUndefined;
//}

//QString PluginHost::hash() const
//{
//    return m_hash;
//}

QString PluginHost::name() const
{
    return m_name;
}

QString PluginHost::version() const
{
    return m_version;
}

QString PluginHost::description() const
{
    return m_description;
}

QString PluginHost::libraryFile() const
{
    return m_libraryFile;
}

QString PluginHost::configFile() const
{
    return m_configFile;
}

//bool PluginHost::isLoaded()
//{
//    if (!m_pluginLoader)
//        return false;
//    return m_pluginLoader->isLoaded ();
//}

//bool PluginHost::unLoad()
//{
//    qDebug()<<Q_FUNC_INFO;
//    if (!m_pluginLoader)
//        m_pluginLoader = new QPluginLoader(m_libraryFile, this);
//    if (m_pluginLoader->isLoaded ()) {
//        if (!m_pluginLoader->unload ()) {
//            qDebug()<<Q_FUNC_INFO<<" fail to unload plugin due to "<<m_pluginLoader->errorString ();
//            return false;
//        }
//    }
//    if (m_pluginLoader)
//        delete m_pluginLoader;
//    m_pluginLoader = nullptr;
//    return true;
//}

//void PluginHost::forceUnload()
//{
//    qDebug()<<Q_FUNC_INFO;
//    if (!m_pluginLoader)
//        m_pluginLoader = new QPluginLoader(m_libraryFile, this);
//    if (m_pluginLoader->isLoaded ()) {
//        if (!m_pluginLoader->unload ())
//            qDebug()<<Q_FUNC_INFO<<" fail to unload plugin due to "<<m_pluginLoader->errorString ();
//    }
//    if (m_pluginObject)
//        delete m_pluginObject;
//    m_pluginObject = nullptr;
//    if (m_pluginLoader)
//        delete m_pluginLoader;
//    m_pluginLoader = nullptr;
//}

//bool PluginHost::isValid()
//{
//    return m_valid;
//}

//template <class T>
//PhoenixPlayer::PluginHost::T *PluginHost::instance()
//{
//    qDebug()<<Q_FUNC_INFO;
//    if (!this->isLoaded ()) {
//        m_pluginObject = m_pluginLoader->instance ();
//    } else {
//        if (!m_pluginObject) {
//            this->unLoad ();
//            if (!m_pluginLoader)
//                m_pluginLoader = new QPluginLoader(m_libraryFile, this);
//            m_pluginObject = m_pluginLoader->instance ();
//        }
//    }
//    qDebug()<<Q_FUNC_INFO<<" here mPluginObject is nullptr "<<(m_pluginObject == 0);
//    return m_pluginObject;
//}

QJsonObject PluginHost::metaObject()
{
    return m_metaObject;
}

//bool PluginHost::operator ==(const PluginHost &other)
//{
//    return this->hash () == other.hash ();
//}

} //namespace PhoenixPlayer {
