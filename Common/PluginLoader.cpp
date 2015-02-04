#include <QHash>
#include <QStringList>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>


#include "PluginLoader.h"
#include "Backend/IPlayBackend.h"
#include "MusicLibrary/IMusicTagParser.h"
#include "MusicLibrary/IPlayListDAO.h"
#include "MetadataLookup/IMetadataLookup.h"

namespace PhoenixPlayer {

PluginLoader *PluginLoader::getInstance()
{
    static PluginLoader loader;
    return &loader;
}

PluginLoader::PluginLoader(QObject *parent)
    : QObject(parent)
{
    //TODO: 根据系统来设置插件的默认路径
    QString path = QString("%1/plugins").arg(QCoreApplication::applicationDirPath());

    //初始化一个空容器
    for (int i = (int)PluginType::TypeAll + 1;
         i < (int)PluginType::TypeLastFlag;
         ++i) {
        mPluginPath.insert (PluginType(i), path);
        mCurrentPluginIndex.insert(PluginType(i), -1);
        mCurrentPluginName.insert(PluginType(i), QString());
    }

//    initPlugins (PluginType::TypeAll);
}

PluginLoader::~PluginLoader()
{
    qDeleteAll (mPlayBackendList);
    if (!mPlayBackendList.isEmpty ())
        mPlayBackendList.clear ();

    qDeleteAll (mPlayListDAOList);
    if (!mPlayListDAOList.isEmpty ())
        mPlayListDAOList.clear ();

    qDeleteAll (mMusicTagParserList);
    if (!mMusicTagParserList.isEmpty ())
        mMusicTagParserList.clear ();

    qDeleteAll (mMetaLookupList);
    if (!mMetaLookupList.isEmpty ())
        mMetaLookupList.clear ();


    if (!mCurrentPluginIndex.isEmpty ())
        mCurrentPluginIndex.clear ();

    if (!mCurrentPluginName.isEmpty ())
        mCurrentPluginName.clear ();

    if (!mPluginPath.isEmpty ())
        mPluginPath.clear ();
}

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

    initPlugins (type);
}

PlayBackend::IPlayBackend *PluginLoader::getCurrentPlayBackend()
{
    if (mCurrentPluginIndex[PluginType::TypePlayBackend] < 0) {
        return nullptr;
    }

    qDebug()<<"CurrentBackend index "
           <<mCurrentPluginIndex[PluginType::TypePlayBackend]
          <<" Name "
         <<mCurrentPluginName[PluginType::TypePlayBackend];

    return mPlayBackendList.at (mCurrentPluginIndex[PluginType::TypePlayBackend]);
}

MusicLibrary::IPlayListDAO *PluginLoader::getCurrentPlayListDAO()
{
    if (mCurrentPluginIndex[PluginType::TypePlayListDAO] < 0) {
        return nullptr;
    }

    qDebug()<<"IPlayListDAO index "
           <<mCurrentPluginIndex[PluginType::TypePlayListDAO]
          <<" Name "
         <<mCurrentPluginName[PluginType::TypePlayListDAO];

    return mPlayListDAOList.at ((int)mCurrentPluginIndex[PluginType::TypePlayListDAO]);
}

MusicLibrary::IMusicTagParser *PluginLoader::getCurrentMusicTagParser()
{
    if (mCurrentPluginIndex[PluginType::TypeMusicTagParser] < 0)
        return nullptr;

    qDebug()<<"IMusicTagParser index "
           <<mCurrentPluginIndex[PluginType::TypeMusicTagParser]
          <<" Name "
         <<mCurrentPluginName[PluginType::TypeMusicTagParser];

    return mMusicTagParserList.at ((int)mCurrentPluginIndex[PluginType::TypeMusicTagParser]);
}

MetadataLookup::IMetadataLookup *PluginLoader::getCurrentMetadataLookup()
{
    if (mCurrentPluginIndex[PluginType::TypeMetadataLookup] < 0)
        return nullptr;

    qDebug()<<"ILyricsLookup index "
           <<mCurrentPluginIndex[PluginType::TypeMetadataLookup]
          <<" Name "
         <<mCurrentPluginName[PluginType::TypeMetadataLookup];

    return mMetaLookupList.at ((int)mCurrentPluginIndex[PluginType::TypeMetadataLookup]);
}

QStringList PluginLoader::getPluginNames(PluginLoader::PluginType type)
{
    QStringList list;
    if (type == PluginType::TypeMusicTagParser) {
        foreach (MusicLibrary::IMusicTagParser *parser, mMusicTagParserList) {
            list.append (parser->getPluginName ());
        }
    } else if (type == PluginType::TypePlayBackend) {
        foreach (PlayBackend::IPlayBackend *backend, mPlayBackendList) {
            list.append (backend->getBackendName ());
        }
    } else if (type == PluginType::TypePlayListDAO) {
        foreach (MusicLibrary::IPlayListDAO *dao, mPlayListDAOList) {
            list.append (dao->getPluginName ());
        }
    } else {
        qDebug()<<"Invalid plugin type.";
    }
    return list;
}

void PluginLoader::initPlugins(PluginType type)
{
    switch (type) {
    case PluginType::TypePlayBackend:
        initPlayBackendPlugin ();
        break;
    case PluginType::TypeMetadataLookup:
        initLyricsLookupPlugin ();
        break;
    case PluginType::TypeMusicTagParser:
        initMusicTagParserPlugin ();
        break;
    case PluginType::TypePlayListDAO:
        initPlayListDaoPlugin ();
        break;
    default: {
        initPlayBackendPlugin ();
        initPlayListDaoPlugin ();
        initMusicTagParserPlugin ();
        initLyricsLookupPlugin ();
        break;
    }
    }
}

void PluginLoader::setNewPlugin(PluginLoader::PluginType type,
                                const QString &newPluginName)
{
    if (type == PluginType::TypePlayBackend) {
        if (newPluginName.toLower ()
                == mCurrentPluginName[PluginType::TypePlayBackend].toLower ()) {
            return;
        }

        qDebug() << "change backend to " << newPluginName;

        for (int i=0; i<mPlayBackendList.size (); ++i) {
            PlayBackend::IPlayBackend *interface = mPlayBackendList.at(i);
            QString name = interface->getBackendName().toLower();
            if (name == newPluginName.toLower ()) {
                mCurrentPluginIndex[PluginType::TypePlayBackend] = i;
                mCurrentPluginName[PluginType::TypePlayBackend] = name;
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

        for (int i=0; i<mPlayBackendList.size (); ++i) {
            MusicLibrary::IPlayListDAO *interface = mPlayListDAOList.at(i);
            QString name = interface->getPluginName ().toLower ();
            if (name == newPluginName.toLower ()) {
                mCurrentPluginIndex[PluginType::TypePlayListDAO] = i;
                mCurrentPluginName[PluginType::TypePlayListDAO] = name;
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

        for (int i=0; i<mMusicTagParserList.size (); ++i) {
            MusicLibrary::IMusicTagParser *interface = mMusicTagParserList.at (i);
            QString name = interface->getPluginName ().toLower ();
            if (name == newPluginName.toLower ()) {
                mCurrentPluginIndex[PluginType::TypeMusicTagParser] = i;
                mCurrentPluginName[PluginType::TypeMusicTagParser] = name;
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

        for (int i=0; i<mMetaLookupList.size (); ++i) {
            MetadataLookup::IMetadataLookup *interface = mMetaLookupList.at (i);
            QString name = interface->getPluginName ().toLower ();
            if (name == newPluginName.toLower ()) {
                mCurrentPluginIndex[PluginType::TypeMetadataLookup] = i;
                mCurrentPluginName[PluginType::TypeMetadataLookup] = name;
                emit signalPluginChanged (type);
                break;
            }
        }
    }
}

void PluginLoader::initPlayBackendPlugin()
{
    //删除之前容器中保存的插件
    if (!mPlayBackendList.isEmpty ()) {
        //删除插件本身
        qDeleteAll(mPlayBackendList);
        mPlayBackendList.clear ();
    }
    int index = 0;

    //system plugins
    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
        if (plugin) {
            //播放后端
            PlayBackend::IPlayBackend *backend
                    = qobject_cast<PlayBackend::IPlayBackend*>(plugin);
            if (backend) {
                mPlayBackendList.append (backend);
                //检测当前找到的插件是否是当前使用的插件
                QString name = backend->getBackendName ().toLower ();
                if (name == mCurrentPluginName[PluginType::TypePlayBackend]
                        .toLower ()) {
                    mCurrentPluginIndex[PluginType::TypePlayBackend] = index;
                    mCurrentPluginName[PluginType::TypePlayBackend] = name;
                }
                index++;
            }
        }
    }

    // dynamic plugins
    qDebug()<<"Search plugin in dir "<<mPluginPath[PluginType::TypePlayBackend];
    QDir dir(mPluginPath[PluginType::TypePlayBackend]);
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        QPluginLoader loader(dir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            //播放后端
            PlayBackend::IPlayBackend *backend
                    = qobject_cast<PlayBackend::IPlayBackend*>(plugin);
            if (backend) {
                mPlayBackendList.append (backend);
                //检测当前找到的插件是否是当前使用的插件
                QString name = backend->getBackendName ().toLower ();
                if (name == mCurrentPluginName[PluginType::TypePlayBackend]
                        .toLower ()) {
                    mCurrentPluginIndex[PluginType::TypePlayBackend] = index;
                    mCurrentPluginName[PluginType::TypePlayBackend] = name;
                }
                index++;
            }
        } else {
            qDebug()<<"no plugin for "<<dir.absoluteFilePath(fileName);
        }
    }

    qDebug()<<"Find playbackend num "<<index;

    if (mPlayBackendList.isEmpty ()) {
        mCurrentPluginIndex[PluginType::TypePlayBackend] = -1;
        mCurrentPluginName[PluginType::TypePlayBackend] = QString();
    } else { //得到第一个插件
        mCurrentPluginIndex[PluginType::TypePlayBackend] = 0;
        mCurrentPluginName[PluginType::TypePlayBackend]
                = mPlayBackendList.at (0)->getBackendName ().toLower ();
    }
}

void PluginLoader::initPlayListDaoPlugin()
{
    if (!mPlayListDAOList.isEmpty ()) {
        qDeleteAll(mPlayListDAOList);
        mPlayBackendList.clear ();
    }
    int index = 0;
    // dynamic plugins
    qDebug()<<"Search plugin in dir "<<mPluginPath[PluginType::TypePlayListDAO];
    QDir dir(mPluginPath[PluginType::TypePlayListDAO]);
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        QPluginLoader loader(dir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            //媒体库数据存储后端
            MusicLibrary::IPlayListDAO *dao
                    = qobject_cast<MusicLibrary::IPlayListDAO*>(plugin);
            if (dao) {
                mPlayListDAOList.append (dao);
                //检测当前找到的插件是否是当前使用的插件
                QString name = dao->getPluginName ().toLower ();
                if (name == mCurrentPluginName[PluginType::TypePlayListDAO]
                        .toLower ()) {
                    mCurrentPluginIndex[PluginType::TypePlayListDAO] = index;
                    mCurrentPluginName[PluginType::TypePlayListDAO] = name;
                }
                index++;
            }
        } else {
            qDebug()<<"no plugin for "<<dir.absoluteFilePath(fileName);
        }
    }

    qDebug()<<"Find IPlayListDAO num "<<index;

    if (mPlayListDAOList.isEmpty ()) {
        mCurrentPluginIndex[PluginType::TypePlayListDAO] = -1;
        mCurrentPluginName[PluginType::TypePlayListDAO] = QString();
    } else { //得到第一个插件
        mCurrentPluginIndex[PluginType::TypePlayListDAO] = 0;
        mCurrentPluginName[PluginType::TypePlayListDAO]
                = mPlayListDAOList.at (0)->getPluginName ().toLower ();
    }
}

void PluginLoader::initMusicTagParserPlugin()
{
    if (!mMusicTagParserList.isEmpty ()) {
        qDeleteAll(mMusicTagParserList);
        mMusicTagParserList.clear ();
    }
    int index = 0;
    // dynamic plugins
    qDebug()<<"Search plugin in dir "<<mPluginPath[PluginType::TypeMusicTagParser];
    QDir dir(mPluginPath[PluginType::TypeMusicTagParser]);
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        QPluginLoader loader(dir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            MusicLibrary::IMusicTagParser *parser
                    = qobject_cast<MusicLibrary::IMusicTagParser*>(plugin);
            if (parser) {
                mMusicTagParserList.append (parser);
                //检测当前找到的插件是否是当前使用的插件
                QString name = parser->getPluginName ().toLower ();
                if (name == mCurrentPluginName[PluginType::TypeMusicTagParser]
                        .toLower ()) {
                    mCurrentPluginIndex[PluginType::TypeMusicTagParser] = index;
                    mCurrentPluginName[PluginType::TypeMusicTagParser] = name;
                }
                index++;
            }
        } else {
            qDebug()<<"no plugin for "<<dir.absoluteFilePath(fileName);
        }
    }

    qDebug()<<"Find MusicTagParser num "<<index;

    if (mMusicTagParserList.isEmpty ()) {
        mCurrentPluginIndex[PluginType::TypeMusicTagParser] = -1;
        mCurrentPluginName[PluginType::TypeMusicTagParser] = QString();
    } else { //得到第一个插件
        mCurrentPluginIndex[PluginType::TypeMusicTagParser] = 0;
        mCurrentPluginName[PluginType::TypeMusicTagParser]
                = mMusicTagParserList.at (0)->getPluginName ().toLower ();
    }
}

void PluginLoader::initLyricsLookupPlugin()
{
    if (!mMetaLookupList.isEmpty ()) {
        qDeleteAll(mMetaLookupList);
        mMetaLookupList.clear ();
    }
    int index = 0;
    // dynamic plugins
    qDebug()<<"Search plugin in dir "<<mPluginPath[PluginType::TypeMetadataLookup];
    QDir dir(mPluginPath[PluginType::TypeMetadataLookup]);
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        QPluginLoader loader(dir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            MetadataLookup::IMetadataLookup *lookup
                    = qobject_cast<MetadataLookup::IMetadataLookup*>(plugin);
            if (lookup) {
                mMetaLookupList.append (lookup);
                //检测当前找到的插件是否是当前使用的插件
                QString name = lookup->getPluginName ().toLower ();
                if (name == mCurrentPluginName[PluginType::TypeMetadataLookup]
                        .toLower ()) {
                    mCurrentPluginIndex[PluginType::TypeMetadataLookup] = index;
                    mCurrentPluginName[PluginType::TypeMetadataLookup] = name;
                }
                index++;
            }
        } else {
            qDebug()<<"no plugin for "<<dir.absoluteFilePath(fileName);
        }
    }

    qDebug()<<"Find LyricsLookup num "<<index;

    if (mMetaLookupList.isEmpty ()) {
        mCurrentPluginIndex[PluginType::TypeMetadataLookup] = -1;
        mCurrentPluginName[PluginType::TypeMetadataLookup] = QString();
    } else { //得到第一个插件
        mCurrentPluginIndex[PluginType::TypeMetadataLookup] = 0;
        mCurrentPluginName[PluginType::TypeMetadataLookup]
                = mMetaLookupList.at (0)->getPluginName ().toLower ();
    }
}
}
