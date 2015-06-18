#include "AsyncTagParserMgrWrapper.h"

#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>

#include "TagParserManager.h"
#include "Util.h"
#include "PluginLoader.h"
#include "MusicLibrary/IPlayListDAO.h"
#include "SongMetaData.h"

namespace PhoenixPlayer {
namespace MusicLibrary {
AsyncTagParserMgrWrapper::AsyncTagParserMgrWrapper(QObject *parent) :
    QObject(parent)
{
//#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
//    mPluginLoader = PluginLoader::instance();
//#else
//    mPluginLoader = SingletonPointer<PluginLoader>::instance ();
//#endif

//    mDao = mPluginLoader->getCurrentPlayListDAO ();

    mMetaDataList = nullptr;

    mThread = new QThread(0);
    mTagParserManager = new TagParserManager(0);
    mTagParserManager->moveToThread (mThread);

    connect (mThread, &QThread::started, [&] {
        qDebug()<<Q_FUNC_INFO<<" Thread start, we'll start TagParser now";
        emit started ();

        mTagParserManager->parserImmediately (mMetaDataList);
    });

    connect (mThread, &QThread::finished, [&] {
        qDebug()<<Q_FUNC_INFO<<" mThread finished";

        emit finished ();
    });
    connect (mTagParserManager, &TagParserManager::parserPending, [&] {

    });
    connect (mTagParserManager, &TagParserManager::parserQueueFinished, [&] {
        qDebug()<<Q_FUNC_INFO<<" mTagParserManager finished, we'll try to finish thread";
//        if (mDao) {
//            mDao->beginTransaction ();
//            for (int i=0; i<mMetaDataList->size (); ++i) {
//                qDebug()<<Q_FUNC_INFO<<" insert ["<<mMetaDataList->at (i)->toString ()<<"]";
////                mDao->insertMetaData (d);
//            }
//            mDao->commitTransaction ();
//            qDeleteAll(mMetaDataList);
//            mMetaDataList.clear ();
//        }
        if (mThread != nullptr)
            mThread->quit ();
    });
}

AsyncTagParserMgrWrapper::~AsyncTagParserMgrWrapper()
{
    if (mThread->isRunning ()) {
        mThread->quit ();
        mThread->wait (3 * 60 * 1000);
    }
    mThread->deleteLater ();
    mTagParserManager->deleteLater ();

//    if (!mMetaDataList.isEmpty ()) {
//        qDeleteAll(mMetaDataList);
//        mMetaDataList.clear ();
//    }
}

void AsyncTagParserMgrWrapper::parser(QList<SongMetaData *> *list)
{
    if (!list) {
        qWarning()<<Q_FUNC_INFO<<" list is nullptr, will ignore this function";
        return;
    }
    mMetaDataList = list;
    mThread->start ();
}

//void AsyncTagParserMgrWrapper::addFile(const QString &path, const QString &file, qint64 size)
//{
////    mListLocker.lock ();
//    qDebug()<<Q_FUNC_INFO<<" file [ "<<path<<" "<<file<<" "<<size<<"]";
//    qDebug()<<Q_FUNC_INFO<<" list size "<<mMetaDataList.size ();

//    QString hash = Util::calculateHash (path.toLocal8Bit ()
//                                        + file.toLocal8Bit ()
//                                        + QString::number (size));

//    SongMetaData *data = new SongMetaData(0);
//    data->setMeta (Common::SongMetaTags::E_FileName, file);
//    data->setMeta (Common::SongMetaTags::E_FilePath, path);
//    data->setMeta (Common::SongMetaTags::E_Hash, hash);
//    data->setMeta (Common::SongMetaTags::E_FileSize, size);
//    mMetaDataList.append (data);
////    mListLocker.unlock ();
//}

//void AsyncTagParserMgrWrapper::startParser()
//{
//    qDebug()<<Q_FUNC_INFO;
//    if (mThread->isRunning ()) {
//        qWarning()<<Q_FUNC_INFO<<" Parser is running, will ignore this function";
//        return;
//    }
//    mThread->start ();
//}

bool AsyncTagParserMgrWrapper::isRunning()
{
    return mThread->isRunning ();
}

} //MusicLibrary
} //PhoenixPlayer
