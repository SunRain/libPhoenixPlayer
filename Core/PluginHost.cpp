#include "PluginHost.h"

#include <QPluginLoader>
#include <QJsonObject>
#include <QDebug>

#include "Util.h"

namespace PhoenixPlayer {

PluginHost::PluginHost(const QString &libraryFile, QObject *parent)
    : QObject(parent),
      mLibraryFile(libraryFile),
      mPluginObject(0)
{
    mValid = false;
    mPluginType = Common::PluginType::PluginTypeUndefined;
    mPluginLoader = new QPluginLoader(libraryFile, this);

    QJsonObject obj = mPluginLoader->metaData ();
    if (!obj.isEmpty ()) {
        QString iid = obj.value ("IID").toString ();
        mValid = true;
        if (iid.startsWith ("PhoenixPlayer.PlayBackend")) {
            mPluginType = Common::PluginPlayBackend;
        } else if (iid.startsWith ("PhoenixPlayer.MetadataLookup")) {
            mPluginType = Common::PluginMetadataLookup;
        } else if (iid.startsWith ("PhoenixPlayer.MusicTagParser")) {
            mPluginType = Common::PluginMusicTagParser;
        } else if (iid.startsWith ("PhoenixPlayer.PlayListDAO")) {
            mPluginType = Common::PluginPlayListDAO;
        } else {
            mPluginType = Common::PluginTypeUndefined;
            mValid = false;
        }
        QJsonObject md = obj.value ("MetaData").toObject ();
        if (!md.isEmpty ()) {
            mName = md.value ("name").toString ();
            mVersion = md.value ("version").toString ();
            mDescription = md.value ("description").toString ();
            mHash = Util::calculateHash (libraryFile);
        } else {
            mValid = false;
        }
    } else {
        qDebug()<<Q_FUNC_INFO<<" get metadata error for plugin ["<<mLibraryFile
               <<"] error is ["<<mPluginLoader->errorString ()<<"]";
    }
    if (mPluginLoader->isLoaded ()) {
        if (!mPluginLoader->unload ())
            qDebug()<<Q_FUNC_INFO<<" fail to unload plugin due to "<<mPluginLoader->errorString ();
    }
    delete mPluginLoader;
    mPluginLoader = 0;
}

PluginHost::~PluginHost()
{
    if (mPluginLoader) {
        if (mPluginLoader->isLoaded ())
            mPluginLoader->unload ();
        delete mPluginLoader;
        mPluginLoader = 0;
    }
    if (mPluginObject) {
        delete mPluginObject;
        mPluginObject = 0;
    }
}

Common::PluginType PluginHost::type() const
{
    return mPluginType;
}

QString PluginHost::hash() const
{
    return mHash;
}

QString PluginHost::name() const
{
    return mName;
}

QString PluginHost::version() const
{
    return mVersion;
}

QString PluginHost::description() const
{
    return mDescription;
}

QString PluginHost::libraryFile() const
{
    return mLibraryFile;
}

bool PluginHost::isLoaded()
{
    if (!mPluginLoader)
        mPluginLoader = new QPluginLoader(mLibraryFile, this);
    if (mPluginLoader->isLoaded ())
        return true;
    return false;
}

void PluginHost::unLoad()
{
    if (!mPluginLoader)
        mPluginLoader = new QPluginLoader(mLibraryFile, this);
    if (mPluginLoader->isLoaded ()) {
        if (!mPluginLoader->unload ()) {
            qDebug()<<Q_FUNC_INFO<<" fail to unload plugin due to "<<mPluginLoader->errorString ();
            if (mPluginObject) {
                delete mPluginObject;
                mPluginObject = 0;
            }
        }
    }
//    if (mPluginObject) {
//        delete mPluginObject;
//        mPluginObject = 0;
//    }
    delete mPluginLoader;
    mPluginLoader = 0;
}

bool PluginHost::isValid()
{
    return mValid;
}

QObject *PluginHost::instance()
{
    if (!this->isLoaded ()) {
        mPluginObject = mPluginLoader->instance ();
    } else {
        if (!mPluginObject) {
            this->unLoad ();
            if (!mPluginLoader)
                mPluginLoader = new QPluginLoader(mLibraryFile, this);
            mPluginObject = mPluginLoader->instance ();
        }
    }
    qDebug()<<Q_FUNC_INFO<<" here mPluginObject is nullptr "<<(mPluginObject == 0);
    return mPluginObject;
}

bool PluginHost::operator ==(const PluginHost &other)
{
    return this->hash () == other.hash ();
}

} //namespace PhoenixPlayer {
