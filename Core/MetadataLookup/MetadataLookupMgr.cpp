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
//#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
//    mPluginLoader = PluginLoader::instance();
//    mSettings = Settings::instance ();
//    mUtil = Util::instance ();
//#else
//    mPluginLoader = SingletonPointer<PluginLoader>::instance ();
//    mSettings = SingletonPointer<Settings>::instance ();
//    mUtil = SingletonPointer<Util>::instance ();
//#endif
    m_pluginLoader = PluginLoader::instance ();
    m_settings = Settings::instance ();
    m_util = Util::instance ();

    m_lookupStarted = false;
    m_currentLookup = nullptr;
    m_destructorState = false;
    m_destructorTimer = new QTimer(this);
    m_destructorTimer->setSingleShot (true);
    m_destructorTimer->setInterval (500);
    connect (m_destructorTimer, &QTimer::timeout,
             this, &MetadataLookupMgr::destructor);
}

MetadataLookupMgr::~MetadataLookupMgr()
{
    qDebug()<<Q_FUNC_INFO;

    if (!m_workQueue.isEmpty ()) {
        qDebug()<<"Delete work queue";
        foreach (WorkNode node, m_workQueue) {
            if (node.data) {
                delete node.data;
                node.data = 0;
            }
        }
        m_workQueue.clear ();
    }

//    destructor ();
}

void MetadataLookupMgr::lookup(SongMetaData *data, IMetadataLookup::LookupType type)
{
    if (m_destructorTimer->isActive ())
        m_destructorTimer->stop ();

    if (!m_settings->fetchMetaDataMobileNetwork ()
            && (m_util->getNetworkType () == Util::NetworkType::TypeMobile)) {
        qWarning()<<"Current network type is mobile type and we disabled fetch metadata here";
        m_lookupStarted = false;
        this->emitFinish ();
        return;
    }
    if (!m_currentLookup) {
        if (!m_destructorState) {
            this->initPluginObject ();
        } else {
            if (!destructorState ()) {
                this->initPluginObject ();
            }
        }
    }

    if (!m_currentLookup){
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

    if (m_workQueueLock.tryLock (300)) {
        if (!m_workQueue.contains (node)) {
            m_workQueue.append (node);
        } else {
            delete node.data;
            node.data = 0;
        }
        m_workQueueLock.unlock ();
    }
    qDebug()<<Q_FUNC_INFO<<QString("add node [%1], list size is [%2]")
              .arg (d->getMeta (Common::E_FileName).toString ())
              .arg (m_workQueue.size ());

    if (!m_lookupStarted) {
        if (m_startLookupLock.tryLock (300)) {
            if (!m_lookupStarted)
                m_lookupStarted = true;
            m_startLookupLock.unlock ();
        }
        processNext ();
    }
}

void MetadataLookupMgr::nextLookupPlugin()
{
    if (m_lookupList.size () == 1 || m_currentLookupIndex == -1) { //only one plugin or no plugin
        //所有插件都使用过了
        QString hash = m_currentNode.data->getMeta (Common::SongMetaTags::E_Hash).toString ();
        qDebug()<<Q_FUNC_INFO<<" all plugin used ===> failed for hash "<<hash;
        emit lookupFailed (hash, m_currentLookup->currentLookupFlag ());
        this->processNext ();
    } else {
        if (m_currentLookupIndex < m_lookupList.size ()-1) { //防止溢出
            //指向下一个插件
            m_currentLookupIndex++;
            m_currentLookup = nullptr;
            m_currentLookup = m_lookupList.at (m_currentLookupIndex);
            qDebug()<<Q_FUNC_INFO<<QString("use new plugin [%1] at index [%2] for lookup type [%3], with track [%4]")
                      .arg (m_currentLookup->metaObject ()->className ())
                      .arg (m_currentLookupIndex)
                      .arg (m_currentNode.type)
                      .arg (m_currentNode.data->getMeta (Common::E_FileName).toString ());

            if (m_currentLookup) {
                this->doLookup ();
            }
        } else {
            QString hash = m_currentNode.data->getMeta (Common::SongMetaTags::E_Hash).toString ();
            qDebug()<<Q_FUNC_INFO<<" all plugin used ===> failed for hash "<<hash;
            emit lookupFailed (hash, m_currentLookup->currentLookupFlag ());
            this->processNext ();
        }
    }
}

void MetadataLookupMgr::doLookupSucceed(const QByteArray &result)
{
    qDebug()<<Q_FUNC_INFO<<QString("Lookup succeed with plugin [%1]")
              .arg (m_currentLookup->metaObject ()->className ());

    if (m_currentNode.data) {
        QString hash = m_currentNode.data->getMeta (Common::SongMetaTags::E_Hash).toString ();
        emit lookupSucceed (hash, result, m_currentLookup->currentLookupFlag ());
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

    m_destructorState = true;

    m_currentLookup = nullptr;
    m_currentLookupIndex = -1;

    if (!m_lookupList.isEmpty ()) {
        qDebug()<<Q_FUNC_INFO<<"mLookupList is not clear, clear it";
        foreach (IMetadataLookup *d, m_lookupList) {
            bool ret = QObject::disconnect (d, &IMetadataLookup::lookupFailed,
                                            this, &MetadataLookupMgr::doLookupFailed);

            qDebug()<<Q_FUNC_INFO<<QString("DIS Connect signal [lookupFailed] for plugin [%1] with ret [%2]")
                      .arg (d->metaObject ()->className ()).arg (ret);

            ret = QObject::disconnect (d, &IMetadataLookup::lookupSucceed,
                                       this, &MetadataLookupMgr::doLookupSucceed);

            qDebug()<<Q_FUNC_INFO<<QString("DIS Connect signal [lookupSucceed] for plugin [%1] with ret [%2]")
                      .arg (d->metaObject ()->className ()).arg (ret);
        }
        m_lookupList.clear ();
    }

    foreach (PluginHost *h, m_pluginLoader->getPluginHostList (Common::PluginMetadataLookup)) {
        h->unLoad ();
    }

    m_destructorState = false;
}

bool MetadataLookupMgr::destructorState()
{
    qDebug()<<Q_FUNC_INFO<<"=======";

    QScopedPointer<QTimer> timer(new QTimer(this));
    QEventLoop eventLoop;
    connect (timer.data (), &QTimer::timeout,[&] {
        if (!m_destructorState) {
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
    if (!m_lookupList.isEmpty ()) {
        qDebug()<<Q_FUNC_INFO<<QString("Current we have %1 lookup plugin pointer in tmp list, clear them")
                  .arg (m_lookupList.size ());

        foreach (IMetadataLookup *d, m_lookupList) {
            bool ret = QObject::disconnect (d, &IMetadataLookup::lookupFailed,
                                            this, &MetadataLookupMgr::doLookupFailed);

            qDebug()<<Q_FUNC_INFO<<QString("DIS Connect signal [lookupFailed] for plugin [%1] with ret [%2]")
                      .arg (d->metaObject ()->className ()).arg (ret);

            ret = QObject::disconnect (d, &IMetadataLookup::lookupSucceed,
                                       this, &MetadataLookupMgr::doLookupSucceed);

            qDebug()<<Q_FUNC_INFO<<QString("DIS Connect signal [lookupSucceed] for plugin [%1] with ret [%2]")
                      .arg (d->metaObject ()->className ()).arg (ret);
        }
        m_lookupList.clear ();
    }
    foreach (PluginHost *host, m_pluginLoader->getPluginHostList (Common::PluginMetadataLookup)) {
        if (host->isLoaded ()) {
            qWarning()<<Q_FUNC_INFO<<"here  host is loaded, try to unload before instance";
            host->unLoad ();
        }
        QObject *o = host->instance ();
        if (o) {
            IMetadataLookup *p = qobject_cast<IMetadataLookup *>(o);
            if (p) {
                qDebug()<<" append "<<host->name ();
                m_lookupList.append (p);
            } else {
                host->unLoad ();
            }
        } else {
            host->unLoad ();
        }

    }
    if (m_lookupList.isEmpty ()) {
        m_currentLookupIndex = -1;
        m_currentLookup = nullptr;
    } else {
        m_currentLookup = m_lookupList.first ();
        m_currentLookupIndex = 0;
    }
    if (!m_lookupList.isEmpty ()) {
        foreach (IMetadataLookup *d, m_lookupList) {
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
    qDebug()<<Q_FUNC_INFO<<"queue size is "<<m_workQueue.size ();

    if (!m_currentLookup) {
        qDebug()<<Q_FUNC_INFO<<"[Fatal error] mCurrentLookup not find ,we got some logic error on getting lookup plugin";
        emitFinish ();
        return;
    }

    if (m_workQueue.isEmpty ()) {
        emitFinish ();
        return;
    }

    //将插件指向列表第一个
    m_currentLookupIndex = 0;
    m_currentLookup = m_lookupList.first ();

    qDebug()<<Q_FUNC_INFO<<" do work by plugin "<<m_currentLookup->metaObject ()->className ();

    bool haveNode = false;
    if (m_workQueueLock.tryLock (300)) {
        if (!m_workQueue.isEmpty ()) {
            m_currentNode = m_workQueue.takeFirst ();
            haveNode = true;
            m_workQueueLock.unlock ();
        }
    }
    if (haveNode)
        this->doLookup ();
}

void MetadataLookupMgr::doLookup()
{
    qDebug()<<Q_FUNC_INFO;
    if (!m_currentLookup) {
        qDebug()<<"Fatal error, current lookup plugin is nulllptr, we got some logic error on getting lookup plugin";
        emitFinish ();
        return;
    }
    //在外部设置LookupFlag，防止当在nextLookupPlugin里面没有任何一个插件支持当前的flag的时候，在发送失败信号的时候无法得到LookupFlag
    m_currentLookup->setCurrentLookupFlag (m_currentNode.type);
    if (m_currentLookup->supportLookup (m_currentNode.type)) {
        qDebug()<<Q_FUNC_INFO<<QString("Use lookup plugin [%1] for lookup type [%2], with track [%3]")
                  .arg (m_currentLookup->metaObject ()->className ())
                  .arg (m_currentNode.type)
                  .arg (m_currentNode.data->getMeta (Common::E_FileName).toString ());

        m_currentLookup->lookup (m_currentNode.data);
    } else {
        this->nextLookupPlugin ();
    }
}

void MetadataLookupMgr::emitFinish()
{
    if (m_startLookupLock.tryLock (300)) {
        m_lookupStarted = false;
        m_startLookupLock.unlock ();

        qDebug()<<Q_FUNC_INFO<<"try to destructor";
//        QTimer::singleShot (1000, this, &MetadataLookupMgr::destructor);
        m_destructorTimer->start ();
    }
    emit queueFinished ();
}

} //MetadataLookup
} //PhoenixPlayer
