#include "MetadataLookup/MetadataLookupMgr.h"

#include <QDebug>
#include <QEventLoop>
#include <QMutex>
#include <QTimer>

#include "Settings.h"
#include "Util.h"
#include "PluginLoader.h"
#include "PluginHost.h"
#include "SingletonPointer.h"
#include "SongMetaData.h"

namespace PhoenixPlayer {
namespace MetadataLookup {
MetadataLookupMgr::MetadataLookupMgr(QObject *parent) :
    QObject(parent)
{
#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    mPluginLoader = PluginLoader::instance();
    mSettings = Settings::instance ();
    mUtil = Util::instance ();
#else
    mPluginLoader = SingletonPointer<PluginLoader>::instance ();
    mSettings = SingletonPointer<Settings>::instance ();
    mUtil = SingletonPointer<Util>::instance ();
#endif
    mLookupStarted = false;
    mCurrentLookup = nullptr;
    mDestructorState = false;
    mDestructorTimer = new QTimer(this);
    mDestructorTimer->setSingleShot (true);
    mDestructorTimer->setInterval (500);
    connect (mDestructorTimer, &QTimer::timeout,
             this, &MetadataLookupMgr::destructor);
}

MetadataLookupMgr::~MetadataLookupMgr()
{
    qDebug()<<Q_FUNC_INFO;

    if (!mWorkQueue.isEmpty ()) {
        qDebug()<<"Delete work queue";
        foreach (WorkNode node, mWorkQueue) {
            if (node.data) {
                delete node.data;
                node.data = 0;
            }
        }
        mWorkQueue.clear ();
    }
    if (!mFailList.isEmpty())
        mFailList.clear();

//    destructor ();
}

void MetadataLookupMgr::lookup(SongMetaData *data, IMetadataLookup::LookupType type)
{
    if (mDestructorTimer->isActive ())
        mDestructorTimer->stop ();

    if (!mSettings->fetchMetaDataMobileNetwork ()
            && (mUtil->getNetworkType () == Util::NetworkType::TypeMobile)) {
        qWarning()<<"Current network type is mobile type and we disabled fetch metadata here";
        mLookupStarted = false;
        this->emitFinish ();
        return;
    }
    if (!mCurrentLookup) {
        if (!mDestructorState) {
            this->initPluginObject ();
        } else {
            if (!destructorState ()) {
                this->initPluginObject ();
            }
        }
    }

    if (!mCurrentLookup){
        qDebug()<<Q_FUNC_INFO<<"MetadataLookupManager we can't found any lookup plugin now";
        emit lookupFailed (data->getMeta(Common::E_Hash).toString(), type);
        return;
    }
    FailNode f;
    f.hash = data->getMeta(Common::SongMetaTags::E_Hash).toString();
    f.type = type;
    if (!mFailList.contains(f)) {
        SongMetaData *d = new SongMetaData(this);
        for (int i = (int)Common::SongMetaTags::E_FirstFlag + 1;
             i < (int)Common::SongMetaTags::E_LastFlag;
             ++ i) {
            d->setMeta (Common::SongMetaTags(i),
                        data->getMeta (Common::SongMetaTags(i)));
        }
        WorkNode node;
        node.data = d;
        node.type = type;

        if (mWorkQueueLock.tryLock (300)) {
            if (!mWorkQueue.contains (node)) {
                mWorkQueue.append (node);
            } else {
                delete node.data;
                node.data = nullptr;
            }
            mWorkQueueLock.unlock ();
        }
        qDebug()<<Q_FUNC_INFO<<QString("add node [%1], list size is [%2]")
                  .arg (d->getMeta (Common::E_FileName).toString ())
                  .arg (mWorkQueue.size ());
    } else {
        qDebug()<<Q_FUNC_INFO
               <<QString("current node [%1] had failed to fetch type [%2] in previous queue")
                 .arg(data->getMeta(Common::SongMetaTags::E_FileName).toString())
                 .arg(type);
    }
    if (!mLookupStarted) {
        if (mStartLookupLock.tryLock (300)) {
            if (!mLookupStarted)
                mLookupStarted = true;
            mStartLookupLock.unlock ();
        }
        processNext ();
    }
}

void MetadataLookupMgr::nextLookupPlugin()
{
    if (mLookupList.size () == 1 || mCurrentLookupIndex == -1) { //only one plugin or no plugin
        //所有插件都使用过了
        QString hash = mCurrentNode.data->getMeta (Common::SongMetaTags::E_Hash).toString ();
        qDebug()<<Q_FUNC_INFO<<" all plugin used ===> failed for hash "<<hash;
        FailNode node;
        node.hash = hash;
        node.type = mCurrentLookup->currentLookupFlag();
        mFailList.append(node);
        emit lookupFailed (hash, mCurrentLookup->currentLookupFlag ());
        this->processNext ();
    } else {
        if (mCurrentLookupIndex < mLookupList.size ()-1) { //防止溢出
            //指向下一个插件
            mCurrentLookupIndex++;
            mCurrentLookup = nullptr;
            mCurrentLookup = mLookupList.at (mCurrentLookupIndex);
            qDebug()<<Q_FUNC_INFO<<QString("use new plugin [%1] at index [%2] for lookup type [%3], with track [%4]")
                      .arg (mCurrentLookup->metaObject ()->className ())
                      .arg (mCurrentLookupIndex)
                      .arg (mCurrentNode.type)
                      .arg (mCurrentNode.data->getMeta (Common::E_FileName).toString ());

            if (mCurrentLookup) {
                this->doLookup ();
            }
        } else {
            QString hash = mCurrentNode.data->getMeta (Common::SongMetaTags::E_Hash).toString ();
            qDebug()<<Q_FUNC_INFO<<" all plugin used ===> failed for hash "<<hash;
            emit lookupFailed (hash, mCurrentLookup->currentLookupFlag ());
            this->processNext ();
        }
    }
}

void MetadataLookupMgr::doLookupSucceed(const QByteArray &result)
{
    qDebug()<<Q_FUNC_INFO<<QString("Lookup succeed with plugin [%1]")
              .arg (mCurrentLookup->metaObject ()->className ());

    if (mCurrentNode.data) {
        QString hash = mCurrentNode.data->getMeta (Common::SongMetaTags::E_Hash).toString ();
        emit lookupSucceed (hash, result, mCurrentLookup->currentLookupFlag ());
    }
    this->processNext ();
}

void MetadataLookupMgr::doLookupFailed()
{
    qDebug()<<Q_FUNC_INFO;
    this->nextLookupPlugin ();
}

void MetadataLookupMgr::destructor()
{
    qDebug()<<Q_FUNC_INFO;

    mDestructorState = true;

    mCurrentLookup = nullptr;
    mCurrentLookupIndex = -1;

    if (!mLookupList.isEmpty ()) {
        qDebug()<<Q_FUNC_INFO<<"mLookupList is not clear, clear it";
        foreach (IMetadataLookup *d, mLookupList) {
            bool ret = QObject::disconnect (d, &IMetadataLookup::lookupFailed,
                                            this, &MetadataLookupMgr::doLookupFailed);

            qDebug()<<Q_FUNC_INFO<<QString("DIS Connect signal [lookupFailed] for plugin [%1] with ret [%2]")
                      .arg (d->metaObject ()->className ()).arg (ret);

            ret = QObject::disconnect (d, &IMetadataLookup::lookupSucceed,
                                       this, &MetadataLookupMgr::doLookupSucceed);

            qDebug()<<Q_FUNC_INFO<<QString("DIS Connect signal [lookupSucceed] for plugin [%1] with ret [%2]")
                      .arg (d->metaObject ()->className ()).arg (ret);
        }
        mLookupList.clear ();
    }

    foreach (PluginHost *h, mPluginLoader->getPluginHostList (Common::PluginMetadataLookup)) {
        h->unLoad ();
    }

    mDestructorState = false;
}

bool MetadataLookupMgr::destructorState()
{
    qDebug()<<Q_FUNC_INFO<<"=======";

    QScopedPointer<QTimer> timer(new QTimer(this));
    QEventLoop eventLoop;
    connect (timer.data (), &QTimer::timeout,[&] {
        if (!mDestructorState) {
            timer.data ()->stop ();
            eventLoop.quit ();
        }
    });
    timer.data ()->setSingleShot (false);
    timer.data ()->start (30);
    eventLoop.exec ();
    return false;
}

void MetadataLookupMgr::initPluginObject()
{
    if (!mLookupList.isEmpty ()) {
        qDebug()<<Q_FUNC_INFO<<QString("Current we have %1 lookup plugin pointer in tmp list, clear them")
                  .arg (mLookupList.size ());

        foreach (IMetadataLookup *d, mLookupList) {
            bool ret = QObject::disconnect (d, &IMetadataLookup::lookupFailed,
                                            this, &MetadataLookupMgr::doLookupFailed);

            qDebug()<<Q_FUNC_INFO<<QString("DIS Connect signal [lookupFailed] for plugin [%1] with ret [%2]")
                      .arg (d->metaObject ()->className ()).arg (ret);

            ret = QObject::disconnect (d, &IMetadataLookup::lookupSucceed,
                                       this, &MetadataLookupMgr::doLookupSucceed);

            qDebug()<<Q_FUNC_INFO<<QString("DIS Connect signal [lookupSucceed] for plugin [%1] with ret [%2]")
                      .arg (d->metaObject ()->className ()).arg (ret);
        }
        mLookupList.clear ();
    }
    foreach (PluginHost *host, mPluginLoader->getPluginHostList (Common::PluginMetadataLookup)) {
        if (host->isLoaded ()) {
            qWarning()<<Q_FUNC_INFO<<"here  host is loaded, try to unload before instance";
            host->unLoad ();
        }
        QObject *o = host->instance ();
        if (o) {
            IMetadataLookup *p = qobject_cast<IMetadataLookup *>(o);
            if (p) {
                qDebug()<<" append "<<host->name ();
                mLookupList.append (p);
            } else {
                host->unLoad ();
            }
        } else {
            host->unLoad ();
        }

    }
    if (mLookupList.isEmpty ()) {
        mCurrentLookupIndex = -1;
        mCurrentLookup = nullptr;
    } else {
        mCurrentLookup = mLookupList.first ();
        mCurrentLookupIndex = 0;
    }
    if (!mLookupList.isEmpty ()) {
        foreach (IMetadataLookup *d, mLookupList) {
            bool ret = QObject::connect (d, &IMetadataLookup::lookupFailed,
                                         this, &MetadataLookupMgr::doLookupFailed,
                                         Qt::UniqueConnection);

            qDebug()<<Q_FUNC_INFO<<QString("connet signal [lookupFailed] for plugin [%1] with ret [%2]")
                      .arg (d->metaObject ()->className ()).arg (ret);

            ret = QObject::connect (d, &IMetadataLookup::lookupSucceed,
                                    this, &MetadataLookupMgr::doLookupSucceed,
                                    Qt::UniqueConnection);

            qDebug()<<Q_FUNC_INFO<<QString("connet signal [lookupSucceed] for plugin [%1] with ret [%2]")
                      .arg (d->metaObject ()->className ()).arg (ret);
        }
    }
}

void MetadataLookupMgr::processNext()
{
    qDebug()<<Q_FUNC_INFO<<"queue size is "<<mWorkQueue.size ();

    if (!mCurrentLookup) {
        qDebug()<<Q_FUNC_INFO<<"[Fatal error] mCurrentLookup not find ,we got some logic error on getting lookup plugin";
        emitFinish ();
        return;
    }

    if (mWorkQueue.isEmpty ()) {
        emitFinish ();
        return;
    }

    //将插件指向列表第一个
    mCurrentLookupIndex = 0;
    mCurrentLookup = mLookupList.first ();

    qDebug()<<Q_FUNC_INFO<<" do work by plugin "<<mCurrentLookup->metaObject ()->className ();

    bool haveNode = false;
    if (mWorkQueueLock.tryLock (300)) {
        if (!mWorkQueue.isEmpty ()) {
            mCurrentNode = mWorkQueue.takeFirst ();
            haveNode = true;
            mWorkQueueLock.unlock ();
        }
    }
    if (haveNode)
        this->doLookup ();
}

void MetadataLookupMgr::doLookup()
{
    qDebug()<<Q_FUNC_INFO;
    if (!mCurrentLookup) {
        qDebug()<<"Fatal error, current lookup plugin is nulllptr, we got some logic error on getting lookup plugin";
        emitFinish ();
        return;
    }
    //在外部设置LookupFlag，防止当在nextLookupPlugin里面没有任何一个插件支持当前的flag的时候，在发送失败信号的时候无法得到LookupFlag
    mCurrentLookup->setCurrentLookupFlag (mCurrentNode.type);
    if (mCurrentLookup->supportLookup (mCurrentNode.type)) {
        qDebug()<<Q_FUNC_INFO<<QString("Use lookup plugin [%1] for lookup type [%2], with track [%3]")
                  .arg (mCurrentLookup->metaObject ()->className ())
                  .arg (mCurrentNode.type)
                  .arg (mCurrentNode.data->getMeta (Common::E_FileName).toString ());

        mCurrentLookup->lookup (mCurrentNode.data);
    } else {
        this->nextLookupPlugin ();
    }
}

void MetadataLookupMgr::emitFinish()
{
    if (mStartLookupLock.tryLock (300)) {
        mLookupStarted = false;
        mStartLookupLock.unlock ();

        qDebug()<<Q_FUNC_INFO<<"try to destructor";
//        QTimer::singleShot (1000, this, &MetadataLookupMgr::destructor);
        mDestructorTimer->start ();
    }
    emit queueFinished ();
}

} //MetadataLookup
} //PhoenixPlayer
