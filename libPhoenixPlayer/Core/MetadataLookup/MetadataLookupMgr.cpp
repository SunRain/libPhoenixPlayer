#include "MetadataLookup/MetadataLookupMgr.h"

#include <QDebug>
#include <QEventLoop>
#include <QMutex>
#include <QTimer>
#include <QList>
#include <QCoreApplication>

#include "Settings.h"
#include "Utility.h"
#include "PluginLoader.h"
#include "PluginHost.h"
#include "SingletonPointer.h"
#include "AudioMetaObject.h"
#include "LibPhoenixPlayerMain.h"
#include "MetadataLookup/MetadataLookupHost.h"

namespace PhoenixPlayer {
namespace MetadataLookup {
MetadataLookupMgr::MetadataLookupMgr(QObject *parent)
    : QThread(parent)
{
    m_pluginLoader =  phoenixPlayerLib->pluginLoader ();
    m_settings = phoenixPlayerLib->settings ();
    m_util = Utility::instance ();
    m_finish = true;
    m_doInternalLoop = true;
    m_useNextHost = true;
    initPluginObject ();
}

MetadataLookupMgr::~MetadataLookupMgr()
{
    if (this->isRunning ()) {
        this->quit ();
        this->wait (3*1000);
    }
    if (!m_hostList.isEmpty ()) {
        foreach (HostNode node, m_hostList) {
            if (*node.lookup)
                *node.lookup = nullptr;
            node.lookup = nullptr;
            if (node.host->isLoaded ()) {
                if (!node.host->unLoad ())
                    node.host->forceUnload ();
                node.host->deleteLater ();
                node.host = nullptr;
            }
        }
        m_hostList.clear ();
    }
    if (!m_workQueue.isEmpty ()) {
//        foreach (WorkNode node, m_workQueue) {
//            node.data = nullptr;
//        }
        m_workQueue.clear ();
    }
}

void MetadataLookupMgr::lookup(const AudioMetaObject &data, IMetadataLookup::LookupType type)
{
    if (!m_settings->fetchMetadataOnMobileNetwork ()
            && (m_util->getNetworkType () == Utility::NetworkType::TypeMobile)) {
        qWarning()<<"Current network type is mobile type and we disabled fetch metadata here";
        this->emitFinish ();
        return;
    }

    m_mutex.lock ();

    WorkNode node;
    node.data = data;
    node.type = type;

//    m_mutex.lock ();
    if (!m_workQueue.contains (node))
        m_workQueue.append (node);
    else
        node.data = AudioMetaObject();
//    m_mutex.unlock ();

    qDebug()<<Q_FUNC_INFO<<QString("add node [%1], list size is [%2]")
              .arg (data.path ()+data.name ()).arg (m_workQueue.size ());

    if (!this->isRunning ())
        this->start ();

    m_mutex.unlock ();
}

void MetadataLookupMgr::run()
{
    while (!m_finish && !m_hostList.isEmpty ()) {
        if (m_workQueue.isEmpty ())
            break;
        m_mutex.lock ();
        m_currentWork = m_workQueue.takeFirst ();
        m_mutex.unlock ();
        for(int i=0; i<m_hostList.size (); ++i) {
//            m_currentHost = m_hostList.first ();
//            m_currentHostIndex = 0;
            if (i == m_hostList.size () -1) {
                emit lookupFailed (m_currentWork.data, (*m_currentHost.lookup)->currentLookupFlag ());
                break;
            }
            if (!m_useNextHost)
                break;
            m_doInternalLoop = true;
            m_useNextHost = true;
            m_currentHost = m_hostList.at (i);
//            在外部设置LookupFlag，防止当在nextLookupPlugin里面没有任何一个插件支持当前的flag的时候，在发送失败信号的时候无法得到LookupFlag
//            if (m_currentLookup->supportLookup (m_currentNode.type)) {
//                qDebug()<<Q_FUNC_INFO<<QString("Use lookup plugin [%1] for lookup type [%2], with track [%3]")
//                          .arg (m_currentLookup->metaObject ()->className ())
//                          .arg (m_currentNode.type)
//                          .arg (m_currentNode.data->getMeta (Common::E_FileName).toString ());

//                m_currentLookup->lookup (m_currentNode.data);
//            } else {
//                this->nextLookupPlugin ();
//            }
            if ((*m_currentHost.lookup)->supportLookup (m_currentWork.type)) {
                qDebug()<<Q_FUNC_INFO<<QString("Use lookup plugin [%1] for lookup type [%2], with track [%3]")
                          .arg ((*m_currentHost.lookup)->metaObject ()->className ())
                          .arg (m_currentWork.type)
                          .arg (/*(*m_currentWork.data)->path ()*/m_currentWork.data.path ()
                                +"/"
                                +/*(*m_currentWork.data)->name ()*/m_currentWork.data.name ());
                (*m_currentHost.lookup)->lookup (m_currentWork.data);
            } else {
                continue;
            }
            //使用一个内部的QEventLoop来等待lookup成功活在失败
            QScopedPointer<QTimer> timer(new QTimer(this));
            QEventLoop eventLoop;
            connect (timer.data (), &QTimer::timeout,[&] {
                if (!m_doInternalLoop) {
                    timer.data ()->stop ();
                    eventLoop.quit ();
                }
            });
            timer.data ()->setSingleShot (false);
            timer.data ()->start (30);
            eventLoop.exec ();
//            emit lookupSucceed (m_currentWork.data, (*m_currentHost.lookup)->currentLookupFlag ());
        }
    }
    emitFinish ();
}

//void MetadataLookupMgr::nextLookupPlugin()
//{
//    if (m_hostList.size () == 1 || m_currentLookupIndex == -1) { //only one plugin or no plugin
//        //所有插件都使用过了
//        QString hash = m_currentWork.data->getMeta (Common::SongMetaTags::E_Hash).toString ();
//        qDebug()<<Q_FUNC_INFO<<" all plugin used ===> failed for hash "<<hash;
//        emit lookupFailed (hash, m_currentLookup->currentLookupFlag ());
//        this->processNext ();
//    } else {
//        if (m_currentLookupIndex < m_hostList.size ()-1) { //防止溢出
//            //指向下一个插件
//            m_currentLookupIndex++;
//            m_currentLookup = nullptr;
//            m_currentLookup = m_hostList.at (m_currentLookupIndex);
//            qDebug()<<Q_FUNC_INFO<<QString("use new plugin [%1] at index [%2] for lookup type [%3], with track [%4]")
//                      .arg (m_currentLookup->metaObject ()->className ())
//                      .arg (m_currentLookupIndex)
//                      .arg (m_currentWork.type)
//                      .arg (m_currentWork.data->getMeta (Common::E_FileName).toString ());

//            if (m_currentLookup) {
//                this->doLookup ();
//            }
//        } else {
//            QString hash = m_currentWork.data->getMeta (Common::SongMetaTags::E_Hash).toString ();
//            qDebug()<<Q_FUNC_INFO<<" all plugin used ===> failed for hash "<<hash;
//            emit lookupFailed (hash, m_currentLookup->currentLookupFlag ());
//            this->processNext ();
//        }
//    }
//}

void MetadataLookupMgr::doLookupSucceed(const QByteArray &result)
{
    qDebug()<<Q_FUNC_INFO<<QString("Lookup succeed with plugin [%1]")
              .arg ((*m_currentHost.lookup)->metaObject ()->className ());

    if (/*(*m_currentWork.data)*/!m_currentWork.data.isEmpty () && !result.isEmpty ()) {
        IMetadataLookup::LookupType type =  (*m_currentHost.lookup)->currentLookupFlag ();
        switch (type) {
        case IMetadataLookup::TypeAlbumDate:{
//            (*m_currentWork.data)->albumMeta ()->setDate (QString(result));
            AlbumMeta m = m_currentWork.data.albumMeta ();
            m.setDate (QString(result));
            m_currentWork.data.setAlbumMeta (m);
            break;
        }
        case IMetadataLookup::TypeAlbumDescription: {
//            (*m_currentWork.data)->albumMeta ()->setDescription (QString(result));
            AlbumMeta m = m_currentWork.data.albumMeta ();
            m.setDescription (QString(result));
            m_currentWork.data.setAlbumMeta (m);
            break;
        }
        case IMetadataLookup::TypeAlbumImage: {
//            (*m_currentWork.data)->albumMeta ()->setImgUri (QString(result));
            AlbumMeta m = m_currentWork.data.albumMeta ();
            m.setImgUri (QString(result));
            m_currentWork.data.setAlbumMeta (m);
            break;
        }
        case IMetadataLookup::TypeArtistDescription: {
//            (*m_currentWork.data)->artistMeta ()->setDescription (QString(result));
            ArtistMeta m = m_currentWork.data.artistMeta ();
            m.setDescription (QString(result));
            m_currentWork.data.setArtistMeta (m);
            break;
        }
        case IMetadataLookup::TypeArtistImage: {
//            (*m_currentWork.data)->artistMeta ()->setImgUri (QString(result));
            ArtistMeta m = m_currentWork.data.artistMeta ();
            m.setImgUri (QString(result));
            m_currentWork.data.setArtistMeta (m);
            break;
        }
            //TODO 分离lyrics路径或者数据
        case IMetadataLookup::TypeLyrics: {
//            (*m_currentWork.data)->setLyricsData (QString(result));
//            (*m_currentWork.data)->setLyricsUri (QString(result));
            m_currentWork.data.setLyricsData (QString(result));
            m_currentWork.data.setLyricsUri (QString(result));
            break;
        }
        case IMetadataLookup::TypeTrackDescription: {
//            (*m_currentWork.data)->trackMeta ()->setDescription (QString(result));
            TrackMeta m = m_currentWork.data.trackMeta ();
            m.setDescription (QString(result));
            m_currentWork.data.setTrackMeta (m);
            break;
        }
        default:
            break;
        }
        emit lookupSucceed (m_currentWork.data, (*m_currentHost.lookup)->currentLookupFlag ());
    }
    m_useNextHost = false;
    m_doInternalLoop = false;
}

void MetadataLookupMgr::doLookupFailed()
{
    qDebug()<<Q_FUNC_INFO;
    m_useNextHost = true;
    m_doInternalLoop = false;
}

//void MetadataLookupMgr::destructor()
//{
//    qDebug()<<Q_FUNC_INFO;

//    m_destructorState = true;

//    m_currentLookup = nullptr;
//    m_currentLookupIndex = -1;

//    if (!m_hostList.isEmpty ()) {
//        qDebug()<<Q_FUNC_INFO<<"mLookupList is not clear, clear it";
//        foreach (IMetadataLookup *d, m_hostList) {
//            bool ret = QObject::disconnect (d, &IMetadataLookup::lookupFailed,
//                                            this, &MetadataLookupMgr::doLookupFailed);

//            qDebug()<<Q_FUNC_INFO<<QString("DIS Connect signal [lookupFailed] for plugin [%1] with ret [%2]")
//                      .arg (d->metaObject ()->className ()).arg (ret);

//            ret = QObject::disconnect (d, &IMetadataLookup::lookupSucceed,
//                                       this, &MetadataLookupMgr::doLookupSucceed);

//            qDebug()<<Q_FUNC_INFO<<QString("DIS Connect signal [lookupSucceed] for plugin [%1] with ret [%2]")
//                      .arg (d->metaObject ()->className ()).arg (ret);
//        }
//        m_hostList.clear ();
//    }

//    foreach (PluginHost *h, m_pluginLoader->getPluginHostList (Common::PluginMetadataLookup)) {
//        h->unLoad ();
//    }

//    m_destructorState = false;
//}

//bool MetadataLookupMgr::destructorState()
//{
//    qDebug()<<Q_FUNC_INFO<<"=======";

//    QScopedPointer<QTimer> timer(new QTimer(this));
//    QEventLoop eventLoop;
//    connect (timer.data (), &QTimer::timeout,[&] {
//        if (!m_destructorState) {
//            timer.data ()->stop ();
//            eventLoop.quit ();
//        }
//    });
//    timer.data ()->setSingleShot (false);
//    timer.data ()->start (30);
//    eventLoop.exec ();
//    return false;
//}

void MetadataLookupMgr::initPluginObject()
{
    if (!m_hostList.isEmpty ()) {
        qDebug()<<Q_FUNC_INFO<<QString("Current we have %1 lookup plugin pointer in tmp list, clear them")
                  .arg (m_hostList.size ());

        foreach (HostNode node, m_hostList) {

            bool ret = QObject::disconnect (*(node.lookup), &IMetadataLookup::lookupFailed,
                                            this, &MetadataLookupMgr::doLookupFailed);

            qDebug()<<Q_FUNC_INFO<<QString("DIS Connect signal [lookupFailed] for plugin [%1] with ret [%2]")
                      .arg ((*(node.lookup))->metaObject ()->className ()).arg (ret);

            ret = QObject::disconnect (*(node.lookup), &IMetadataLookup::lookupSucceed,
                                       this, &MetadataLookupMgr::doLookupSucceed);

            qDebug()<<Q_FUNC_INFO<<QString("DIS Connect signal [lookupSucceed] for plugin [%1] with ret [%2]")
                      .arg ((*(node.lookup))->metaObject ()->className ()).arg (ret);

            node.lookup = nullptr;
            if (node.host->isLoaded ()) {
                if (!node.host->unLoad ())
                    node.host->forceUnload ();
            }
            node.host->deleteLater ();
            node.host = nullptr;
        }
        m_hostList.clear ();
    }

    QStringList libs = m_settings->metadataLookupLibraries ();
    foreach (QString s, libs) {
        MetadataLookupHost *host = new MetadataLookupHost(s, this);
        if (host->isValid ()) {
            IMetadataLookup *lookup = host->instance<IMetadataLookup>();
            if (lookup) {
                HostNode node;
                node.lookup = &lookup;
                node.host = host;
                m_hostList.append (node);
                continue;
            }
            if (host->isLoaded ()) {
                if (!host->unLoad ())
                    host->forceUnload ();
            }
        }
        host->deleteLater ();
        host = nullptr;
    }
//    m_currentHostIndex = -1;
    if (!m_hostList.isEmpty ()) {
//        m_currentHost = m_hostList.first ();
//        m_currentHostIndex = 0;
        foreach (HostNode node, m_hostList) {
            bool ret = QObject::connect (*(node.lookup), &IMetadataLookup::lookupFailed,
                                         this, &MetadataLookupMgr::doLookupFailed,
                                         Qt::UniqueConnection);

            qDebug()<<Q_FUNC_INFO<<QString("connet signal [lookupFailed] for plugin [%1] with ret [%2]")
                      .arg ((*(node.lookup))->metaObject ()->className ()).arg (ret);

            ret = QObject::connect (*(node.lookup), &IMetadataLookup::lookupSucceed,
                                    this, &MetadataLookupMgr::doLookupSucceed,
                                    Qt::UniqueConnection);

            qDebug()<<Q_FUNC_INFO<<QString("connet signal [lookupSucceed] for plugin [%1] with ret [%2]")
                      .arg ((*(node.lookup))->metaObject ()->className ()).arg (ret);
        }
    }
}

//void MetadataLookupMgr::processNext()
//{
//    qDebug()<<Q_FUNC_INFO<<"queue size is "<<m_workQueue.size ();

//    if (!m_currentLookup) {
//        qDebug()<<Q_FUNC_INFO<<"[Fatal error] mCurrentLookup not find ,we got some logic error on getting lookup plugin";
//        emitFinish ();
//        return;
//    }

//    if (m_workQueue.isEmpty ()) {
//        emitFinish ();
//        return;
//    }

//    //将插件指向列表第一个
//    m_currentLookupIndex = 0;
//    m_currentLookup = m_hostList.first ();

//    qDebug()<<Q_FUNC_INFO<<" do work by plugin "<<m_currentLookup->metaObject ()->className ();

//    bool haveNode = false;
//    if (m_workQueueLock.tryLock (300)) {
//        if (!m_workQueue.isEmpty ()) {
//            m_currentWork = m_workQueue.takeFirst ();
//            haveNode = true;
//            m_workQueueLock.unlock ();
//        }
//    }
//    if (haveNode)
//        this->doLookup ();
//}

//void MetadataLookupMgr::doLookup()
//{
//    qDebug()<<Q_FUNC_INFO;
//    if (!m_currentLookup) {
//        qDebug()<<"Fatal error, current lookup plugin is nulllptr, we got some logic error on getting lookup plugin";
//        emitFinish ();
//        return;
//    }
//    //在外部设置LookupFlag，防止当在nextLookupPlugin里面没有任何一个插件支持当前的flag的时候，在发送失败信号的时候无法得到LookupFlag
//    m_currentLookup->setCurrentLookupFlag (m_currentWork.type);
//    if (m_currentLookup->supportLookup (m_currentWork.type)) {
//        qDebug()<<Q_FUNC_INFO<<QString("Use lookup plugin [%1] for lookup type [%2], with track [%3]")
//                  .arg (m_currentLookup->metaObject ()->className ())
//                  .arg (m_currentWork.type)
//                  .arg (m_currentWork.data->getMeta (Common::E_FileName).toString ());

//        m_currentLookup->lookup (m_currentWork.data);
//    } else {
//        this->nextLookupPlugin ();
//    }
//}

void MetadataLookupMgr::emitFinish()
{
//    if (m_startLookupLock.tryLock (300)) {
//        m_lookupStarted = false;
//        m_startLookupLock.unlock ();

//        qDebug()<<Q_FUNC_INFO<<"try to destructor";
////        QTimer::singleShot (1000, this, &MetadataLookupMgr::destructor);
//        m_destructorTimer->start ();
//    }
//    emit queueFinished ();
    if (this->isRunning ()) {
        m_mutex.lock ();
        m_finish = true;
        this->quit ();
        this->wait (3*1000);
        m_mutex.unlock ();
    }
    emit queueFinished ();
}

} //MetadataLookup
} //PhoenixPlayer
