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
    for (int i = (int)PluginType::TypeAll + 1;
         i < (int)PluginType::TypeLastFlag;
         ++i) {
        mPluginPath.insert (PluginType(i), path);
//        mCurrentPluginIndex.insert(PluginType(i), -1);
        mCurrentPluginName.insert(PluginType(i), QString());
    }

    mPlayBackendLoader = new QPluginLoader(this);
    mDaoLoader = new QPluginLoader(this);
    mTagParserLoader = new QPluginLoader(this);
    mMetaLookupLoader = new QPluginLoader(this);

    if (!isInit)
        initPlugins ();
}

PluginLoader::~PluginLoader()
{
    qDebug()<<__FUNCTION__;
    if (!mPluginList.isEmpty())
        mPluginList.clear();
    if (!mCurrentPluginName.isEmpty ())
        mCurrentPluginName.clear ();
    if (!mPluginPath.isEmpty ())
        mPluginPath.clear ();
}

#ifdef SAILFISH_OS
PluginLoader *PluginLoader::instance()
{
    qDebug()<<">>>>>>>>"<<__FUNCTION__<<"<<<<<<<<<<<<<<";
    static QMutex mutex;
    static QScopedPointer<PluginLoader> scp;

    if (Q_UNLIKELY(scp.isNull())) {
        qDebug()<<">>>>>>>> statr new";
        mutex.lock();
        scp.reset(new PluginLoader(0));
        mutex.unlock();
    }
    qDebug()<<">>>>>>>> return "<<scp.data()->metaObject()->className();;
    return scp.data();
}
#endif

void PluginLoader::setPluginPath(PluginLoader::PluginType type, const QString &path)
{
    if (type == PluginType::TypeAll) {
        for (int i = (int)PluginType::TypeAll + 1;
             i < (int)PluginType::TypeLastFlag;
             ++i) {
            mPluginPath[PluginType(i)] = path;
        }
    } else {
        mPluginPath[type] = path;
    }

    initPlugins ();
}

PlayBackend::IPlayBackend *PluginLoader::getCurrentPlayBackend()
{
    if (mPluginList.isEmpty()) {
        qDebug()<<__FUNCTION__<<" mPluginList is empty";
        return nullptr;
    }
    QString f;
    foreach (PluginObject obj, mPluginList) {
        if (mCurrentPluginName[PluginType::TypePlayBackend].isEmpty()) {
            if (obj.type == PluginType::TypePlayBackend) {
                f = obj.file;
                mCurrentPluginName[PluginType::TypePlayBackend] = obj.name;
                break;
            }
        } else {
            if ((obj.name.toLower() == mCurrentPluginName[PluginType::TypePlayBackend].toLower())
                    && (obj.type == PluginType::TypePlayBackend)) {
                f = obj.file;
                break;
            }
        }
    }

    qDebug()<<"CurrentBackend index Name "
         <<mCurrentPluginName[PluginType::TypePlayBackend];

    if (mPlayBackendLoader->isLoaded ()) {
        if (!mPlayBackendLoader->unload ())
            qDebug()<<__FUNCTION__<<mPlayBackendLoader->errorString ();
    }
    //QPluginLoader loader(f);
    mPlayBackendLoader->setFileName (f);
    QObject *plugin = mPlayBackendLoader->instance ();//loader.instance();
    if (plugin) {
        IPlayBackend *p = qobject_cast<IPlayBackend*>(plugin);
        if (p) {
            return p;
        } else {
            qDebug()<<__FUNCTION__<<" cant change to IPlayBackend";
            return nullptr;
        }
    } else {
        qDebug()<<__FUNCTION__<<" cant load plugin "<<mPlayBackendLoader->errorString();
        return nullptr;
    }
}

MusicLibrary::IPlayListDAO *PluginLoader::getCurrentPlayListDAO()
{
    if (mPluginList.isEmpty()) {
        qDebug()<<__FUNCTION__<<" mPluginList is empty";
        return nullptr;
    }
    QString f;
    foreach (PluginObject obj, mPluginList) {
        if (mCurrentPluginName[PluginType::TypePlayListDAO].isEmpty()) {
            if (obj.type == PluginType::TypePlayListDAO) {
                f = obj.file;
                mCurrentPluginName[PluginType::TypePlayListDAO] = obj.name;
                break;
            }
        } else {
            if ((obj.name.toLower() == mCurrentPluginName[PluginType::TypePlayListDAO].toLower())
                    && (obj.type == PluginType::TypePlayListDAO)) {
                f = obj.file;
                break;
            }
        }
    }

    qDebug()<<"Current IPlayListDAO index Name "
         <<mCurrentPluginName[PluginType::TypePlayListDAO];

    if (mDaoLoader->isLoaded ()) {
        if (!mDaoLoader->unload ())
            qDebug()<<__FUNCTION__<<mDaoLoader->errorString ();
    }
    mDaoLoader->setFileName (f);
//    QPluginLoader loader(f);
    QObject *plugin = mDaoLoader->instance ();//loader.instance();
    if (plugin) {
        IPlayListDAO *p = qobject_cast<IPlayListDAO*>(plugin);
        if (p) {
            return p;
        } else {
            qDebug()<<__FUNCTION__<<" cant change to TypePlayListDAO";
            return nullptr;
        }
    } else {
        qDebug()<<__FUNCTION__<<" cant load plugin "<<mDaoLoader->errorString();
        return nullptr;
    }
}

MusicLibrary::IMusicTagParser *PluginLoader::getCurrentMusicTagParser()
{
    if (mPluginList.isEmpty()) {
        qDebug()<<__FUNCTION__<<" mPluginList is empty";
        return nullptr;
    }
    QString f;
    foreach (PluginObject obj, mPluginList) {
        if (mCurrentPluginName[PluginType::TypeMusicTagParser].isEmpty()) {
            if (obj.type == PluginType::TypeMusicTagParser) {
                f = obj.file;
                mCurrentPluginName[PluginType::TypeMusicTagParser] = obj.name;
                break;
            }
        } else {
            if ((obj.name.toLower() == mCurrentPluginName[PluginType::TypeMusicTagParser].toLower())
                    && (obj.type == PluginType::TypeMusicTagParser)) {
                f = obj.file;
                break;
            }
        }
    }

    qDebug()<<"Current TypeMusicTagParser index Name "
         <<mCurrentPluginName[PluginType::TypeMusicTagParser];

    if (mTagParserLoader->isLoaded ()) {
        if (!mTagParserLoader->unload ())
            qDebug()<<__FUNCTION__<<mTagParserLoader->errorString ();
    }
    mTagParserLoader->setFileName (f);
//    QPluginLoader loader(f);
    QObject *plugin = mTagParserLoader->instance ();//loader.instance();
    if (plugin) {
        IMusicTagParser *p = qobject_cast<IMusicTagParser*>(plugin);
        if (p) {
            return p;
        } else {
            qDebug()<<__FUNCTION__<<" cant change to IMusicTagParser";
            return nullptr;
        }
    } else {
        qDebug()<<__FUNCTION__<<" cant load plugin "<<mTagParserLoader->errorString();
        return nullptr;
    }
}

IMetadataLookup *PluginLoader::getCurrentMetadataLookup()
{
    if (mPluginList.isEmpty()) {
        qDebug()<<__FUNCTION__<<" mPluginList is empty";
        return nullptr;
    }
    QString f;
    foreach (PluginObject obj, mPluginList) {
        if (mCurrentPluginName[PluginType::TypeMetadataLookup].isEmpty()) {
            if (obj.type == PluginType::TypeMetadataLookup) {
                f = obj.file;
                mCurrentPluginName[PluginType::TypeMetadataLookup] = obj.name;
                break;
            }
        } else {
            if ((obj.name.toLower() == mCurrentPluginName[PluginType::TypeMetadataLookup].toLower())
                    && (obj.type == PluginType::TypeMetadataLookup)) {
                f = obj.file;
                break;
            }
        }
    }

    qDebug()<<"Current IMetadataLookup index Name "
         <<mCurrentPluginName[PluginType::TypeMetadataLookup];

    if (mMetaLookupLoader->isLoaded ()) {
        if (!mMetaLookupLoader->unload ())
            qDebug()<<__FUNCTION__<<mMetaLookupLoader->errorString ();
    }
    mMetaLookupLoader->setFileName (f);
//    QPluginLoader loader(f);
    QObject *plugin = mMetaLookupLoader->instance ();//loader.instance();
    if (plugin) {
        IMetadataLookup *p = qobject_cast<IMetadataLookup*>(plugin);
        if (p) {
            return p;
        } else {
            qDebug()<<__FUNCTION__<<" cant change to IMetadataLookup";
            return nullptr;
        }
    } else {
        qDebug()<<__FUNCTION__<<" cant load plugin "<<mMetaLookupLoader->errorString();
        return nullptr;
    }
}

QStringList PluginLoader::getPluginNames(PluginLoader::PluginType type)
{
    QStringList list;
    if (mPluginList.isEmpty()) {
        qDebug()<<__FUNCTION__<<" Can't find plugins";
        return list;
    }
    switch (type) {
    case PluginType::TypeMetadataLookup: {
        foreach (PluginObject obj, mPluginList) {
            if (obj.type == PluginType::TypeMetadataLookup)
                list.append(obj.name);
        }
        break;
    }
    case PluginType::TypeMusicTagParser: {
        foreach (PluginObject obj, mPluginList) {
            if (obj.type == PluginType::TypeMusicTagParser)
                list.append(obj.name);
        }
        break;
    }
    case PluginType::TypePlayBackend: {
        foreach (PluginObject obj, mPluginList) {
            if (obj.type == PluginType::TypePlayBackend)
                list.append(obj.name);
        }
        break;
    }
    case PluginType::TypePlayListDAO: {
        foreach (PluginObject obj, mPluginList) {
            if (obj.type == PluginType::TypePlayListDAO)
                list.append(obj.name);
        }
        break;
    }
    default:
        qDebug()<<"Invalid plugin type.";
        break;
    }
    return list;
}

void PluginLoader::initPlugins()
{
    //删除之前容器中保存的插件
    if (!mPluginList.isEmpty()) {
        mPluginList.clear();
    }
    // dynamic plugins
    qDebug()<<"Search plugin in dir "<<mPluginPath[PluginType::TypePlayBackend];
    QDir dir(mPluginPath[PluginType::TypePlayBackend]);
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        QString absFilePath = dir.absoluteFilePath(fileName);
        QPluginLoader loader(absFilePath);
        QObject *plugin = loader.instance();
        if (plugin) {
            //播放后端
            if (IPlayBackend *obj = qobject_cast<IPlayBackend*>(plugin)) {
                PluginObject p;
                p.type = PluginType::TypePlayBackend;
                p.description = obj->getDescription();
                p.file = absFilePath;
                p.name = obj->getBackendName();
                p.version = obj->getBackendVersion();
                mPluginList.append(p);
            } else if (IMetadataLookup *obj = qobject_cast<IMetadataLookup*>(plugin)) {
                PluginObject p;
                p.description = obj->getDescription();
                p.file = absFilePath;
                p.name = obj->getPluginName();
                p.type = PluginType::TypeMetadataLookup;
                p.version = obj->getPluginVersion();
                mPluginList.append(p);
            } else if (IPlayListDAO *obj = qobject_cast<IPlayListDAO*>(plugin)) {
                PluginObject p;
                p.description = obj->getDescription();
                p.file = absFilePath;
                p.name = obj->getPluginName();
                p.type = PluginType::TypePlayListDAO;
                p.version = obj->getPluginVersion();
                mPluginList.append(p);
            } else if (IMusicTagParser *obj = qobject_cast<IMusicTagParser*>(plugin)) {
                PluginObject p;
                p.description = obj->getDescription();
                p.file = absFilePath;
                p.name = obj->getPluginName();
                p.type = PluginType::TypeMusicTagParser;
                p.version = obj->getPluginVersion();
                mPluginList.append(p);
            } else {
                qDebug()<<__FUNCTION__<<" can't change to plugin";
            }
        } else {
            qDebug()<<__FUNCTION__<<" load plugin error "<<loader.errorString();
        }
        if (loader.isLoaded()) {
            qDebug()<<__FUNCTION__<<" try to unload plugin now";
            if (!loader.unload())
                qDebug()<<__FUNCTION__<<" unload plugin fail";
        }
    }

    foreach (PluginObject obj, mPluginList) {
        qDebug()<<"Found plugins for type ["
               <<obj.type<<"] name ["
              <<obj.name<<"] file ["
             <<obj.file<<"]";
    }

    isInit = true;
}

void PluginLoader::setNewPlugin(PluginLoader::PluginType type,
                                const QString &newPluginName)
{
    if (mPluginList.isEmpty()) {
        qDebug()<<__FUNCTION__<<" mPluginList is empty!!!!";
        return;
    }
    if (type == PluginType::TypePlayBackend) {
        if (newPluginName.toLower ()
                == mCurrentPluginName[PluginType::TypePlayBackend].toLower ()) {
            return;
        }

        qDebug() << "change backend to " << newPluginName;
        if (mPlayBackendLoader->isLoaded ()) {
           if (!mPlayBackendLoader->unload ())
               qDebug()<<__FUNCTION__<<mPlayBackendLoader->errorString ();
        }
        foreach (PluginObject p, mPluginList) {
            if (p.type == PluginType::TypePlayBackend
                    && p.name.toLower() == newPluginName.toLower()) {
                mCurrentPluginName[PluginType::TypePlayBackend] = p.name;
                emit signalPluginChanged (type);
                break;
            }
        }
    }
    if (type == PluginType::TypePlayListDAO) {
        if (newPluginName.toLower ()
                == mCurrentPluginName[PluginType::TypePlayListDAO].toLower ()) {
            return;
        }

        qDebug() << "change play list dao to " << newPluginName;

        if (mDaoLoader->isLoaded ()) {
            if (!mDaoLoader->unload ())
                qDebug()<<__FUNCTION__<<mDaoLoader->errorString ();
        }
        foreach (PluginObject p, mPluginList) {
            if (p.type == PluginType::TypePlayListDAO
                    && p.name.toLower() == newPluginName.toLower()) {
                mCurrentPluginName[PluginType::TypePlayListDAO] = p.name;
                emit signalPluginChanged (type);
                break;
            }
        }
    }
    if (type == PluginType::TypeMusicTagParser) {
        if (newPluginName.toLower ()
                == mCurrentPluginName[PluginType::TypeMusicTagParser].toLower ()) {
            return;
        }

        qDebug() << "change music tag parser to " << newPluginName;

        if (mTagParserLoader->isLoaded ()) {
            if (!mTagParserLoader->unload ())
                qDebug()<<__FUNCTION__<<mTagParserLoader->errorString ();
        }

        foreach (PluginObject p, mPluginList) {
            if (p.type == PluginType::TypeMusicTagParser
                    && p.name.toLower() == newPluginName.toLower()) {
                mCurrentPluginName[PluginType::TypeMusicTagParser] = p.name;
                emit signalPluginChanged (type);
                break;
            }
        }
    }
    if (type == PluginType::TypeMetadataLookup) {
        if (newPluginName.toLower ()
                == mCurrentPluginName[PluginType::TypeMetadataLookup].toLower ()) {
            return;
        }

        qDebug() << "change TypeMetadataLookup to " << newPluginName;

        if (mMetaLookupLoader->isLoaded ()) {
            if (!mMetaLookupLoader->unload ())
                qDebug()<<__FUNCTION__<<mMetaLookupLoader->errorString ();
        }

        foreach (PluginObject p, mPluginList) {
            if (p.type == PluginType::TypeMetadataLookup
                    && p.name.toLower() == newPluginName.toLower()) {
                mCurrentPluginName[PluginType::TypeMetadataLookup] = p.name;
                emit signalPluginChanged (type);
                break;
            }
        }
    }
}
}
