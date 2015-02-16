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

    if (!isInit)
        initPlugins ();
}

PluginLoader::~PluginLoader()
{
    qDebug()<<__FUNCTION__;

//    qDeleteAll (mPlayBackendList);
//    if (!mPlayBackendList.isEmpty ())
//        mPlayBackendList.clear ();

//    qDeleteAll (mPlayListDAOList);
//    if (!mPlayListDAOList.isEmpty ())
//        mPlayListDAOList.clear ();

//    qDeleteAll (mMusicTagParserList);
//    if (!mMusicTagParserList.isEmpty ())
//        mMusicTagParserList.clear ();

    //TODO:why this makes crash?
//    qDeleteAll (mMetaLookupList);
//    if (!mMetaLookupList.isEmpty ()) {
//        mMetaLookupList.clear ();
//    }
    if (!mPluginList.isEmpty())
        mPluginList.clear();

//    if (!mCurrentPluginIndex.isEmpty ())
//        mCurrentPluginIndex.clear ();

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

    QPluginLoader loader(f);
    QObject *plugin = loader.instance();
    if (plugin) {
        IPlayBackend *p = qobject_cast<IPlayBackend*>(plugin);
        if (p) {
            return p;
        } else {
            qDebug()<<__FUNCTION__<<" cant change to IPlayBackend";
            return nullptr;
        }
    } else {
        qDebug()<<__FUNCTION__<<" cant load plugin "<<loader.errorString();
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

    QPluginLoader loader(f);
    QObject *plugin = loader.instance();
    if (plugin) {
        IPlayListDAO *p = qobject_cast<IPlayListDAO*>(plugin);
        if (p) {
            return p;
        } else {
            qDebug()<<__FUNCTION__<<" cant change to TypePlayListDAO";
            return nullptr;
        }
    } else {
        qDebug()<<__FUNCTION__<<" cant load plugin "<<loader.errorString();
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

    QPluginLoader loader(f);
    QObject *plugin = loader.instance();
    if (plugin) {
        IMusicTagParser *p = qobject_cast<IMusicTagParser*>(plugin);
        if (p) {
            return p;
        } else {
            qDebug()<<__FUNCTION__<<" cant change to IMusicTagParser";
            return nullptr;
        }
    } else {
        qDebug()<<__FUNCTION__<<" cant load plugin "<<loader.errorString();
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

    QPluginLoader loader(f);
    QObject *plugin = loader.instance();
    if (plugin) {
        IMetadataLookup *p = qobject_cast<IMetadataLookup*>(plugin);
        if (p) {
            return p;
        } else {
            qDebug()<<__FUNCTION__<<" cant change to IMetadataLookup";
            return nullptr;
        }
    } else {
        qDebug()<<__FUNCTION__<<" cant load plugin "<<loader.errorString();
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

        qDebug() << "change lyricsLookup to " << newPluginName;

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

//void PluginLoader::initPlayBackendPlugin()
//{
//    //删除之前容器中保存的插件
//    if (!mPlayBackendList.isEmpty ()) {
//        //删除插件本身
//        qDeleteAll(mPlayBackendList);
//        mPlayBackendList.clear ();
//    }
//    int index = 0;

//    //system plugins
//    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
//        if (plugin) {
//            //播放后端
//            PlayBackend::IPlayBackend *backend
//                    = qobject_cast<PlayBackend::IPlayBackend*>(plugin);
//            if (backend) {
//                mPlayBackendList.append (backend);
//                //检测当前找到的插件是否是当前使用的插件
//                QString name = backend->getBackendName ().toLower ();
//                if (name == mCurrentPluginName[PluginType::TypePlayBackend]
//                        .toLower ()) {
//                    mCurrentPluginIndex[PluginType::TypePlayBackend] = index;
//                    mCurrentPluginName[PluginType::TypePlayBackend] = name;
//                }
//                index++;
//            }
//        }
//    }

//    // dynamic plugins
//    qDebug()<<"Search plugin in dir "<<mPluginPath[PluginType::TypePlayBackend];
//    QDir dir(mPluginPath[PluginType::TypePlayBackend]);
//    foreach (QString fileName, dir.entryList(QDir::Files)) {
//        QPluginLoader loader(dir.absoluteFilePath(fileName));
//        QObject *plugin = loader.instance();
//        if (plugin) {
//            //播放后端
//            PlayBackend::IPlayBackend *backend
//                    = qobject_cast<PlayBackend::IPlayBackend*>(plugin);
//            if (backend) {
//                mPlayBackendList.append (backend);
//                //检测当前找到的插件是否是当前使用的插件
//                QString name = backend->getBackendName ().toLower ();
//                if (name == mCurrentPluginName[PluginType::TypePlayBackend]
//                        .toLower ()) {
//                    mCurrentPluginIndex[PluginType::TypePlayBackend] = index;
//                    mCurrentPluginName[PluginType::TypePlayBackend] = name;
//                }
//                index++;
//            }
//        } else {
//            qDebug()<<"no plugin for "<<dir.absoluteFilePath(fileName);
//        }
//    }

//    qDebug()<<"Find playbackend num "<<index;

//    if (mPlayBackendList.isEmpty ()) {
//        mCurrentPluginIndex[PluginType::TypePlayBackend] = -1;
//        mCurrentPluginName[PluginType::TypePlayBackend] = QString();
//    } else { //得到第一个插件
//        mCurrentPluginIndex[PluginType::TypePlayBackend] = 0;
//        mCurrentPluginName[PluginType::TypePlayBackend]
//                = mPlayBackendList.at (0)->getBackendName ().toLower ();
//    }
//}

//void PluginLoader::initPlayListDaoPlugin()
//{
//    if (!mPlayListDAOList.isEmpty ()) {
//        qDeleteAll(mPlayListDAOList);
//        mPlayBackendList.clear ();
//    }
//    int index = 0;
//    // dynamic plugins
//    qDebug()<<"Search plugin in dir "<<mPluginPath[PluginType::TypePlayListDAO];
//    QDir dir(mPluginPath[PluginType::TypePlayListDAO]);
//    foreach (QString fileName, dir.entryList(QDir::Files)) {
//        QPluginLoader loader(dir.absoluteFilePath(fileName));
//        QObject *plugin = loader.instance();
//        if (plugin) {
//            //媒体库数据存储后端
//            MusicLibrary::IPlayListDAO *dao
//                    = qobject_cast<MusicLibrary::IPlayListDAO*>(plugin);
//            if (dao) {
//                mPlayListDAOList.append (dao);
//                //检测当前找到的插件是否是当前使用的插件
//                QString name = dao->getPluginName ().toLower ();
//                if (name == mCurrentPluginName[PluginType::TypePlayListDAO]
//                        .toLower ()) {
//                    mCurrentPluginIndex[PluginType::TypePlayListDAO] = index;
//                    mCurrentPluginName[PluginType::TypePlayListDAO] = name;
//                }
//                index++;
//            }
//        } else {
//            qDebug()<<"no plugin for "<<dir.absoluteFilePath(fileName);
//        }
//    }

//    qDebug()<<"Find IPlayListDAO num "<<index;

//    if (mPlayListDAOList.isEmpty ()) {
//        mCurrentPluginIndex[PluginType::TypePlayListDAO] = -1;
//        mCurrentPluginName[PluginType::TypePlayListDAO] = QString();
//    } else { //得到第一个插件
//        mCurrentPluginIndex[PluginType::TypePlayListDAO] = 0;
//        mCurrentPluginName[PluginType::TypePlayListDAO]
//                = mPlayListDAOList.at (0)->getPluginName ().toLower ();
//    }
//}

//void PluginLoader::initMusicTagParserPlugin()
//{
//    if (!mMusicTagParserList.isEmpty ()) {
//        qDeleteAll(mMusicTagParserList);
//        mMusicTagParserList.clear ();
//    }
//    int index = 0;
//    // dynamic plugins
//    qDebug()<<"Search plugin in dir "<<mPluginPath[PluginType::TypeMusicTagParser];
//    QDir dir(mPluginPath[PluginType::TypeMusicTagParser]);
//    foreach (QString fileName, dir.entryList(QDir::Files)) {
//        QPluginLoader loader(dir.absoluteFilePath(fileName));
//        QObject *plugin = loader.instance();
//        if (plugin) {
//            MusicLibrary::IMusicTagParser *parser
//                    = qobject_cast<MusicLibrary::IMusicTagParser*>(plugin);
//            if (parser) {
//                mMusicTagParserList.append (parser);
//                //检测当前找到的插件是否是当前使用的插件
//                QString name = parser->getPluginName ().toLower ();
//                if (name == mCurrentPluginName[PluginType::TypeMusicTagParser]
//                        .toLower ()) {
//                    mCurrentPluginIndex[PluginType::TypeMusicTagParser] = index;
//                    mCurrentPluginName[PluginType::TypeMusicTagParser] = name;
//                }
//                index++;
//            }
//        } else {
//            qDebug()<<"no plugin for "<<dir.absoluteFilePath(fileName);
//        }
//    }

//    qDebug()<<"Find MusicTagParser num "<<index;

//    if (mMusicTagParserList.isEmpty ()) {
//        mCurrentPluginIndex[PluginType::TypeMusicTagParser] = -1;
//        mCurrentPluginName[PluginType::TypeMusicTagParser] = QString();
//    } else { //得到第一个插件
//        mCurrentPluginIndex[PluginType::TypeMusicTagParser] = 0;
//        mCurrentPluginName[PluginType::TypeMusicTagParser]
//                = mMusicTagParserList.at (0)->getPluginName ().toLower ();
//    }
//}

//void PluginLoader::initMetadataLookupPlugin()
//{
//    if (!mMetaLookupList.isEmpty ()) {
//        qDeleteAll(mMetaLookupList);
//        mMetaLookupList.clear ();
//    }
//    int index = 0;
//    // dynamic plugins
//    qDebug()<<"Search plugin in dir "<<mPluginPath[PluginType::TypeMetadataLookup];
//    QDir dir(mPluginPath[PluginType::TypeMetadataLookup]);
//    foreach (QString fileName, dir.entryList(QDir::Files)) {
//        QPluginLoader loader(dir.absoluteFilePath(fileName));
//        QObject *plugin = loader.instance();
//        if (plugin) {
//            IMetadataLookup *lookup = qobject_cast<IMetadataLookup*>(plugin);
//            if (lookup) {
//                mMetaLookupList.append (lookup);
//                //检测当前找到的插件是否是当前使用的插件
//                QString name = lookup->getPluginName ().toLower ();
//                if (name == mCurrentPluginName[PluginType::TypeMetadataLookup]
//                        .toLower ()) {
//                    mCurrentPluginIndex[PluginType::TypeMetadataLookup] = index;
//                    mCurrentPluginName[PluginType::TypeMetadataLookup] = name;
//                }
//                index++;
//            }
//        } else {
//            qDebug()<<"no plugin for "<<dir.absoluteFilePath(fileName);
//        }
//    }

//    qDebug()<<"Find MetadataLookup num "<<index;

//    if (mMetaLookupList.isEmpty ()) {
//        mCurrentPluginIndex[PluginType::TypeMetadataLookup] = -1;
//        mCurrentPluginName[PluginType::TypeMetadataLookup] = QString();
//    } else { //得到第一个插件
//        mCurrentPluginIndex[PluginType::TypeMetadataLookup] = 0;
//        mCurrentPluginName[PluginType::TypeMetadataLookup]
//                = mMetaLookupList.at (0)->getPluginName ().toLower ();
//    }
//}
}
