#include <QObject>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>

#include "PlayBackendLoader.h"

namespace PhoenixPlayer{
namespace PlayBackend {

PlayBackendLoader *PlayBackendLoader::getInstance()
{
    static PlayBackendLoader loader;
    return &loader;
}

void PlayBackendLoader::setBackendPluginPath(const QString &path)
{
    mBackendPath = path;
    initBackend();
}

PlayBackend::IPlayBackend *PlayBackendLoader::getCurrentBackend()
{
    if (mCurrentBackendIndex < 0)
        return 0;
    return mBackendList.at(mCurrentBackendIndex);
}

void PlayBackendLoader::setNewBackend(const QString &newBackendName)
{
    if (newBackendName.toLower() == mCurrentBackendName.toLower())
        return;

    qDebug() << "change backend to " << newBackendName;

    for (int i=0; i<mBackendList.size(); ++i) {
        PlayBackend::IPlayBackend *interface = mBackendList.at(i);
        QString name = interface->getBackendName().toLower();
        if (name == newBackendName.toLower()) {
            mCurrentBackendIndex = i;
            mCurrentBackendName = name;
            emit signalPlayBackendChanged(/*newBackendName*/);
            break;
        }
    }
}

PlayBackendLoader::PlayBackendLoader(QObject *parent)
    : QObject(parent)
{
    //TODO: 根据系统来设置插件的默认路径
    mBackendPath = QString("%1/plugins").arg(QCoreApplication::applicationDirPath());
    mCurrentBackendIndex = -1;
    initBackend();
}

PlayBackendLoader::~PlayBackendLoader()
{
    mBackendList.clear();
}

void PlayBackendLoader::initBackend()
{
    int index = 0;

    mBackendList.clear();

    //system plugins
    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
        if (plugin) {
            PlayBackend::IPlayBackend *interface = qobject_cast<PlayBackend::IPlayBackend*>(plugin);
            if (interface) {
                mBackendList.append(interface);
                QString name = interface->getBackendName().toLower();
                if (name == mCurrentBackendName.toLower()) {
                    mCurrentBackendIndex = index;
                    mCurrentBackendName = name;
                }
                index++;
            }
        }
    }

    // dynamic plugins
    QDir dir(mBackendPath);
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        QPluginLoader loader(dir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            PlayBackend::IPlayBackend *interface = qobject_cast<PlayBackend::IPlayBackend*>(plugin);
            if (interface) {
                mBackendList.append(interface);
                QString name = interface->getBackendName().toLower();
                if (name == mCurrentBackendName.toLower()) {
                    mCurrentBackendIndex = index;
                    mCurrentBackendName = name;
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

    if (mBackendList.isEmpty()) {
        mCurrentBackendName = "";
        mCurrentBackendIndex = -1;
    } else {
        mCurrentBackendIndex = 0;
        mCurrentBackendName = mBackendList.at(0)->getBackendName();
    }
}

} //Core
} //PhoenixPlayer
