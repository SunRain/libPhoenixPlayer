#include "PluginHost.h"

#include <QPluginLoader>
#include <QJsonObject>
#include <QDebug>
#include <QFileInfo>

#include "Util.h"

namespace PhoenixPlayer {

PluginHost::PluginHost(const QString &libraryFile, QObject *parent)
    : QObject(parent),
      m_libraryFile(libraryFile),
      m_pluginObject(0)
{
    m_valid = false;
    m_pluginType = Common::PluginType::PluginTypeUndefined;
    m_pluginLoader = new QPluginLoader(libraryFile, this);
    QFileInfo f(libraryFile);
    QString libraryPath = f.absolutePath ();

    QJsonObject obj = m_pluginLoader->metaData ();
    if (!obj.isEmpty ()) {
        QString iid = obj.value ("IID").toString ();
        m_valid = true;
        if (iid.startsWith ("PhoenixPlayer.PlayBackend")) {
            m_pluginType = Common::PluginPlayBackend;
        } else if (iid.startsWith ("PhoenixPlayer.MetadataLookup")) {
            m_pluginType = Common::PluginMetadataLookup;
        } else if (iid.startsWith ("PhoenixPlayer.MusicTagParser")) {
            m_pluginType = Common::PluginMusicTagParser;
        } else if (iid.startsWith ("PhoenixPlayer.PlayListDAO")) {
            m_pluginType = Common::PluginPlayListDAO;
        } else if (iid.startsWith ("PhoenixPlayer.Decoder")) {
            m_pluginType = Common::PluginDecoder;
        } else if (iid.startsWith ("PhoenixPlayer.OutPut")) {
            m_pluginType = Common::PluginOutPut;
        } else {
            m_pluginType = Common::PluginTypeUndefined;
            m_valid = false;
        }
        QJsonObject md = obj.value ("MetaData").toObject ();
        if (!md.isEmpty ()) {
            m_name = md.value ("name").toString ();
            m_version = md.value ("version").toString ();
            m_description = md.value ("description").toString ();
            m_hash = Util::calculateHash (libraryFile);
            m_configFile = md.value ("configFile").toString ();
            if (!m_configFile.isEmpty ()) {
                m_configFile = QString("%1/%2")
                        .arg (libraryPath).arg (m_configFile);
            }
        } else {
            m_valid = false;
        }
    } else {
        qDebug()<<Q_FUNC_INFO<<" get metadata error for plugin ["<<m_libraryFile
               <<"] error is ["<<m_pluginLoader->errorString ()<<"]";
    }
    if (m_pluginLoader->isLoaded ()) {
        if (!m_pluginLoader->unload ())
            qDebug()<<Q_FUNC_INFO<<" fail to unload plugin due to "<<m_pluginLoader->errorString ();
    }
    delete m_pluginLoader;
    m_pluginLoader = 0;
}

PluginHost::~PluginHost()
{
    if (m_pluginLoader) {
        if (m_pluginLoader->isLoaded ())
            m_pluginLoader->unload ();
        delete m_pluginLoader;
        m_pluginLoader = 0;
    }
    if (m_pluginObject) {
        delete m_pluginObject;
        m_pluginObject = 0;
    }
}

Common::PluginType PluginHost::type() const
{
    return m_pluginType;
}

QString PluginHost::hash() const
{
    return m_hash;
}

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

bool PluginHost::isLoaded()
{
    if (!m_pluginLoader)
        m_pluginLoader = new QPluginLoader(m_libraryFile, this);
    if (m_pluginLoader->isLoaded ())
        return true;
    return false;
}

void PluginHost::unLoad()
{
    qDebug()<<Q_FUNC_INFO;
    if (!m_pluginLoader)
        m_pluginLoader = new QPluginLoader(m_libraryFile, this);
    if (m_pluginLoader->isLoaded ()) {
        if (!m_pluginLoader->unload ()) {
            qDebug()<<Q_FUNC_INFO<<" fail to unload plugin due to "<<m_pluginLoader->errorString ();
            if (m_pluginObject) {
                delete m_pluginObject;
                m_pluginObject = 0;
            }
        }
    }
//    if (mPluginObject) {
//        delete mPluginObject;
//        mPluginObject = 0;
//    }
    delete m_pluginLoader;
    m_pluginLoader = 0;
}

bool PluginHost::isValid()
{
    return m_valid;
}

QObject *PluginHost::instance()
{
    qDebug()<<Q_FUNC_INFO;
    if (!this->isLoaded ()) {
        m_pluginObject = m_pluginLoader->instance ();
    } else {
        if (!m_pluginObject) {
            this->unLoad ();
            if (!m_pluginLoader)
                m_pluginLoader = new QPluginLoader(m_libraryFile, this);
            m_pluginObject = m_pluginLoader->instance ();
        }
    }
    qDebug()<<Q_FUNC_INFO<<" here mPluginObject is nullptr "<<(m_pluginObject == 0);
    return m_pluginObject;
}

bool PluginHost::operator ==(const PluginHost &other)
{
    return this->hash () == other.hash ();
}

} //namespace PhoenixPlayer {
