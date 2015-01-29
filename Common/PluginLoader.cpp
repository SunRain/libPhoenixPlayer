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
    for (int i = (int)PluginType::TypePlayBackend;
         i <= (int)PluginType::TypePlayListDAO;
         ++i) {
        mPluginPath.insert (PluginType(i), path);
        mCurrentPluginIndex.insert(PluginType(i), -1);
        mCurrentPluginName.insert(PluginType(i), QString());
    }

    initPlugins (PluginType::TypeAll);
}

PluginLoader::~PluginLoader()
{

}

void PluginLoader::setPluginPath(PluginLoader::PluginType type, const QString &path)
{
    if (type == PluginType::TypeAll) {
        for (int i = (int)PluginType::TypePlayBackend;
             i <= (int)PluginType::TypePlayListDAO;
             i++) {
            mPluginPath[type] = path;
        }
    } else {
        mPluginPath[type] = path;
    }

    initPlugins (type);
}

PlayBackend::IPlayBackend *PluginLoader::getPlayBackend()
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

MusicLibrary::IPlayListDAO *PluginLoader::getPlayListDAO()
{
    if (mCurrentPluginIndex[PluginType::TypePlayListDAO] < 0) {
        return nullptr;
    }

    qDebug()<<"IMusicTagParser index "
           <<mCurrentPluginIndex[PluginType::TypePlayListDAO]
          <<" Name "
         <<mCurrentPluginName[PluginType::TypePlayListDAO];

    return mPlayListDAOList.at ((int)mCurrentPluginIndex[PluginType::TypePlayListDAO]);
}

void PluginLoader::initPlugins(PluginType type)
{
    if (type == PluginType::TypePlayBackend) {
        initPlayBackendPlugin ();
    } else if (type == PluginType::TypePlayListDAO) {
        initPlayListDaoPlugin ();
    } else {
        initPlayBackendPlugin ();
        initPlayListDaoPlugin ();
    }
    //TODO:不需要查找系统插件,因为暂时没有系统路径下的插件支持
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
//                    mCurrentPluginIndex[PluginType::TypePlayBackend]
//                            = index[PluginType::TypePlayBackend];
//                    mCurrentPluginName[PluginType::TypePlayBackend] = name;
//                }
//                index[PluginType::TypePlayBackend]
//                        = index[PluginType::TypePlayBackend]++;
//            }
//            //媒体库数据存储后端
//            MusicLibrary::IPlayListDAO *dao
//                    = qobject_cast<MusicLibrary::IPlayListDAO*>(plugin);
//            if (dao) {
//                mPlayListDAOList.append (dao);
//                //检测当前找到的插件是否是当前使用的插件
//                QString name = dao->getPluginName ().toLower ();
//                if (name == mCurrentPluginName[PluginType::TypePlayListDAO]
//                        .toLower ()) {
//                    mCurrentPluginIndex[PluginType::TypePlayListDAO]
//                            = index[PluginType::TypePlayListDAO];
//                    mCurrentPluginName[PluginType::TypePlayListDAO] = name;
//                }
//                index[PluginType::TypePlayListDAO]
//                        = index[PluginType::TypePlayListDAO]++;
//            }
//        }
//    }
}

void PluginLoader::setNewPlugin(PluginLoader::PluginType type, const QString &newPluginName)
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
//                emit signalPluginChanged ((int)type);
                break;
            }
        }
    }
    if (type == PluginType::TypePlayListDAO) {
        if (newPluginName.toLower ()
                == mCurrentPluginName[PluginType::TypePlayListDAO].toLower ()) {
            return;
        }

        qDebug() << "change backend to " << newPluginName;

        for (int i=0; i<mPlayBackendList.size (); ++i) {
            MusicLibrary::IPlayListDAO *interface = mPlayListDAOList.at(i);
            QString name = interface->getPluginName ().toLower ();
            if (name == newPluginName.toLower ()) {
                mCurrentPluginIndex[PluginType::TypePlayListDAO] = i;
                mCurrentPluginName[PluginType::TypePlayListDAO] = name;
                emit signalPluginChanged (type);
//                emit signalPluginChanged ((int)type);
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
}
