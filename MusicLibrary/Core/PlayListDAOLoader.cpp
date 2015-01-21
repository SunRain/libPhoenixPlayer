#include <QCoreApplication>
#include <QPluginLoader>
#include <QtPlugin>
#include <QDebug>
#include <QDir>
#include <qobject.h>

#include "PlayListDAOLoader.h"

namespace PhoenixPlayer {
namespace PlayList {


PlayListDAOLoader::PlayListDAOLoader(QObject *parent) : QObject(parent)
{
    //TODO: 根据系统来设置插件的默认路径
    mPluginPath = QString("%1/plugins").arg(QCoreApplication::applicationDirPath ());
    mCurrentPluginIndex = -1;
    initPlugin ();
}

PlayListDAOLoader *PlayListDAOLoader::getInstance()
{
    static PlayListDAOLoader loader;
    return &loader;
}

PlayListDAOLoader::~PlayListDAOLoader()
{
    qDebug()<<__FUNCTION__;
    //删除List里面的指针类型文件
    qDeleteAll(mPluginList);
    if (!mPluginList.isEmpty ()) {
        mPluginList.clear ();
    }
     qDebug()<<"after "<<__FUNCTION__;
}

void PlayListDAOLoader::setPluginPath(const QString &path)
{
    mPluginPath = path;
    initPlugin ();
}

IPlayListDAO *PlayListDAOLoader::getPlayListDAO()
{
    if (mCurrentPluginIndex < 0)
        return 0;
    return mPluginList.at(mCurrentPluginIndex);
}

void PlayListDAOLoader::setNewPlayListDAO(const QString &pluginName)
{
    if (pluginName.toLower () == mCurrentPluginName.toLower ())
        return;
    qDebug()<<"Change PlayListDAO to "<<pluginName;

    for (int i=0; i<mPluginList.size (); ++i) {
        IPlayListDAO *dao = mPluginList.at (i);
        if (dao->getPluginName ().toLower () == pluginName.toLower ()) {
            mCurrentPluginName = dao->getPluginName ();
            mCurrentPluginIndex = i;
            emit pluginChanged ();
        }
    }
}

void PlayListDAOLoader::initPlugin()
{
    int index = 0;
    mPluginList.clear ();

    //system plugins
    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
        if (plugin) {
            IPlayListDAO *interface = qobject_cast<IPlayListDAO*>(plugin);
            if (interface) {
                mPluginList.append(interface);
                QString name = interface->getPluginName ().toLower ();
                if (name == mCurrentPluginName.toLower()) {
                    mCurrentPluginIndex = index;
                    mCurrentPluginName = name;
                }
                index++;
            }
        }
    }

    // dynamic plugins
    QDir dir(mPluginPath);
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        QPluginLoader loader(dir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            IPlayListDAO *interface = qobject_cast<IPlayListDAO*>(plugin);
            if (interface) {
                mPluginList.append(interface);
                QString name = interface->getPluginName ().toLower ();
                if (name == mCurrentPluginName.toLower()) {
                    mCurrentPluginIndex = index;
                    mCurrentPluginName = name;
                }
                index++;
            } else {
                qDebug()<<"cant qobject_cast for "<<dir.absoluteFilePath(fileName);
            }
        } else {
            qDebug()<<"no plugin for "<<dir.absoluteFilePath(fileName);
        }
    }

    qDebug()<<"Find plugins num "<<index;

    if (mPluginList.isEmpty()) {
        mCurrentPluginName = "";
        mCurrentPluginIndex = -1;
    } else {
        mCurrentPluginIndex = 0;
        mCurrentPluginName = mPluginList.at(0)->getPluginName ();
    }
}

} //PlayList
} //PhoenixPlayer
