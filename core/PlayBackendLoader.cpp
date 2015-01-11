#include <QObject>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>
#include <QtPlugin>

#include "PlayBackendLoader.h"

PhoenixPlayerCore::PlayBackendLoader *PhoenixPlayerCore::PlayBackendLoader::getInstance()
{
    static PhoenixPlayerCore::PlayBackendLoader loader;
    return &loader;
}

void PhoenixPlayerCore::PlayBackendLoader::setBackendPluginPath(const QString &path)
{
    mBackendPath = path;
    initBackend();
}

PhoenixPlayerCore::IPlayBackend *PhoenixPlayerCore::PlayBackendLoader::getCurrentBackend()
{
    if (mCurrentBackendIndex < 0)
        return 0;
    return mBackendList.at(mCurrentBackendIndex);
}

//PhoenixPlayerCore::BasePlayBackendInterface *PhoenixPlayerCore::PlayBackendLoader::getBackend(const QString &backendName)
//{
//    if (backendName.toLower() == mCurrentBackendName.toLower())
//        return mBackendList.at(mCurrentBackendIndex);

//    qDebug() << "try to change backend to " << backendName;

//    for (int i=0; i<mBackendList.size(); ++i) {
//        BasePlayBackendInterface *interface = mBackendList.at(i);
//        QString name = interface->getBackendName().toLower();
//        if (name == backendName.toLower()) {
//            mCurrentBackendIndex = i;
//            mCurrentBackendName = name;
//            break;
//        }
//    }
//    return mBackendList.at(mCurrentBackendIndex);
//}

void PhoenixPlayerCore::PlayBackendLoader::setNewBackend(const QString &newBackendName)
{
    if (newBackendName.toLower() == mCurrentBackendName.toLower())
        return;

    qDebug() << "change backend to " << newBackendName;

    for (int i=0; i<mBackendList.size(); ++i) {
        IPlayBackend *interface = mBackendList.at(i);
        QString name = interface->getBackendName().toLower();
        if (name == newBackendName.toLower()) {
            mCurrentBackendIndex = i;
            mCurrentBackendName = name;
            emit signalPlayBackendChanged(/*newBackendName*/);
            break;
        }
    }
}

PhoenixPlayerCore::PlayBackendLoader::PlayBackendLoader(QObject *parent)
{
    mBackendPath = QString("%1/PlayBackend").arg(QCoreApplication::applicationDirPath());
    mCurrentBackendIndex = -1;
    initBackend();
}

PhoenixPlayerCore::PlayBackendLoader::~PlayBackendLoader()
{
    mBackendList.clear();
}

void PhoenixPlayerCore::PlayBackendLoader::initBackend()
{
    int index = 0;

    mBackendList.clear();

    //system plugins
    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
        if (plugin) {
            IPlayBackend *interface = qobject_cast<IPlayBackend*>(plugin);
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
        qDebug()<<"=== found file "<<dir.absoluteFilePath(fileName);

        QPluginLoader loader(dir.absoluteFilePath(fileName));

        qDebug()<<"=== file loader "<<loader.isLoaded()
               <<" "
              <<loader.errorString();

        QObject *plugin = loader.instance();
        if (plugin) {
            IPlayBackend *interface = qobject_cast<IPlayBackend*>(plugin);
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

    qDebug()<<"Find plugins num "<<index;

    if (mBackendList.isEmpty()) {
        mCurrentBackendName = "";
        mCurrentBackendIndex = -1;
    }
}















