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

    destructor ();
}

void MetadataLookupMgr::lookup(SongMetaData *data, IMetadataLookup::LookupType type)
{
    if (!mSettings->fetchMetaDataMobileNetwork ()
            && (mUtil->getNetworkType () == Util::NetworkType::TypeMobile)) {
        qWarning()<<"Current network type is mobile type and we disabled fetch metadata here";
        mLookupStarted = false;
        emit queueFinished ();
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
            node.data = 0;
        }
        mWorkQueueLock.unlock ();
    }
    qDebug()<<Q_FUNC_INFO<<QString("add node [%1], list size is [%2]")
              .arg (d->getMeta (Common::E_FileName).toString ())
              .arg (mWorkQueue.size ());

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
        emit lookupFailed (hash, mCurrentLookup->currentLookupFlag ());
        this->processNext ();
    } else {
        if (mCurrentLookupIndex < mLookupList.size ()-1) { //防止溢出
            //指向下一个插件
            mCurrentLookupIndex++;
            mCurrentLookup = mLookupList.at (mCurrentLookupIndex);
            qDebug()<<Q_FUNC_INFO<<QString("use new plugin [%1] at index [%2] for lookup type [%3], with track [%4]")
                      .arg (mCurrentLookup->metaObject ()->className ())
                      .arg (mCurrentLookupIndex)
                      .arg (mCurrentNode.type)
                      .arg (mCurrentNode.data->getMeta (Common::E_FileName).toString ());

            if (mCurrentLookup)
                this->doLookup ();
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

void MetadataLookupMgr::destructor()
{
    qDebug()<<Q_FUNC_INFO;

    mDestructorState = true;

    mCurrentLookup = nullptr;
    mCurrentLookupIndex = -1;

    foreach (PluginHost *h, mHostList) {
        h->unLoad ();
    }
    mHostList.clear ();
    /*
     * 直接将list里面的指针设置为nullptr而不是delete它们，这是由于这些是指向系统插件的指针，
     * 我们希望由pluginhost来管理他们，而不是单纯的delete
     *
     */
    foreach (IMetadataLookup *o, mLookupList) {
        if (o != nullptr)
            o = nullptr;
    }
    mLookupList.clear ();

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
        mLookupList.clear ();
    }
    mHostList = mPluginLoader->getPluginHostList (Common::PluginMetadataLookup);
    foreach (PluginHost *host, mHostList) {
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
    foreach (IMetadataLookup *lookup, mLookupList) {
        bool ret = QObject::connect (lookup, &IMetadataLookup::lookupFailed,
                                     this, &MetadataLookupMgr::nextLookupPlugin,
                                     Qt::UniqueConnection);
        qDebug()<<Q_FUNC_INFO<<QString("connet signal [lookupFailed] for plugin [%1] with ret [%2]")
                  .arg (lookup->metaObject ()->className ()).arg (ret);

        ret = QObject::connect (lookup, &IMetadataLookup::lookupSucceed,
                                this, &MetadataLookupMgr::doLookupSucceed,
                                Qt::UniqueConnection);
        qDebug()<<Q_FUNC_INFO<<QString("connet signal [lookupSucceed] for plugin [%1] with ret [%2]")
                  .arg (lookup->metaObject ()->className ()).arg (ret);

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
    if (mCurrentLookup->supportLookup (mCurrentNode.type)) {
        qDebug()<<Q_FUNC_INFO<<QString("Use lookup plugin [%1] for lookup type [%2], with track [%3]")
                  .arg (mCurrentLookup->metaObject ()->className ())
                  .arg (mCurrentNode.type)
                  .arg (mCurrentNode.data->getMeta (Common::E_FileName).toString ());

        mCurrentLookup->setCurrentLookupFlag (mCurrentNode.type);
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

        this->destructor ();
    }
    emit queueFinished ();
}

} //MetadataLookup
} //PhoenixPlayer
