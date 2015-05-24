
#include "MetadataLookup/MetadataLookupManager.h"

#include <QDebug>
#include <QMutex>
#include <QTimer>

#include "Common.h"
#include "MetadataLookup/IMetadataLookup.h"
#include "PluginLoader.h"
#include "SongMetaData.h"
#include "SingletonPointer.h"
#include "PluginHost.h"

namespace PhoenixPlayer {

namespace MetadataLookup {
using namespace PhoenixPlayer;

MetadataLookupManager::MetadataLookupManager(QObject *parent) : QObject(parent)
{
#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    mPluginLoader = PluginLoader::instance();
#else
    mPluginLoader = SingletonPointer<PluginLoader>::instance ();
#endif
    mLookupStarted = false;
//    mBackupLookup = nullptr;
    mLookup = 0;
//    mPreConnection = QString();
    initPluginObject ();
    initPluginNames ();
}

MetadataLookupManager::~MetadataLookupManager()
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

    if (!mPluginList.isEmpty ()) {
        qDebug()<<"=== delete plugin list";
        foreach (IMetadataLookup *o, mPluginList) {
            if (o) {
                delete o;
                o = 0;
            }
        }
        mPluginList.clear ();
    }

    qDebug()<<"after "<<Q_FUNC_INFO;
}

void MetadataLookupManager::lookup(SongMetaData *data,
                                   IMetadataLookup::LookupType type)
{
    if (mLookup == 0){
        qDebug()<<"MetadataLookupManager we can't found any lookup plugin now";
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

    mMutex.lock ();
    if (!mWorkQueue.contains (node)) {
        mWorkQueue.append (node);
    } else {
        delete node.data;
        node.data = 0;
    }
    mMutex.unlock ();

    qDebug()<<Q_FUNC_INFO<<" add node "<<d->getMeta (Common::E_FileName).toString ()
              <<" list sieze "<<mWorkQueue.size ();
    if (!mLookupStarted) {
        mMutex.lock ();
        mLookupStarted = true;
        mMutex.unlock ();
        processNext ();
    }
}

//void MetadataLookupManager::slotDoLookup()
//{
//    qDebug()<<Q_FUNC_INFO<<" do work by plugin "<<mLookup->getPluginName ();
//    //将插件指向列表第一个
//    mCurrentIndex = 0;
//    mLookup = mPluginList.first ();

//    mMutex.lock ();
//    mCurrentNode = mWorkQueue.takeFirst ();
//    mMutex.unlock ();
//    doLookup ();
//}

void MetadataLookupManager::nextLookupPlugin()
{
    if (mPluginList.size () == 1 || mCurrentIndex == -1) { //only one plugin
        //所有插件都使用过了
        QString hash = mCurrentNode.data
                ->getMeta (Common::SongMetaTags::E_Hash).toString ();
        qDebug()<<Q_FUNC_INFO<<" all plugin used ===> failed for hash "<<hash;
        emit lookupFailed (hash, mLookup->currentLookupFlag ());
        processNext ();
    } else {
        if (mCurrentIndex < mPluginList.size ()-1) { //防止溢出

            bool ret = mLookup->disconnect ();
            qDebug()<<"************ "<<Q_FUNC_INFO<<" disconnect lookupFailed ret "<<ret;

            //指向下一个插件
            mCurrentIndex++;
            mLookup = mPluginList.at (mCurrentIndex);
            qDebug()<<Q_FUNC_INFO<<" new plugin "<<mPluginList.at (mCurrentIndex)->getPluginName ()
                   <<" index "<<mCurrentIndex;

            if (mLookup != 0)
                doLookup ();
        } else {
            QString hash = mCurrentNode.data
                    ->getMeta (Common::SongMetaTags::E_Hash).toString ();
            qDebug()<<Q_FUNC_INFO<<" all plugin used ===> failed for hash "<<hash;
            emit lookupFailed (hash, mLookup->currentLookupFlag ());
            processNext ();
        }
    }
}

void MetadataLookupManager::initPluginObject()
{
    qDebug()<<Q_FUNC_INFO;

    QList<PluginHost*> hostList = mPluginLoader->getPluginHostList (Common::PluginMetadataLookup);
    foreach (PluginHost *host, hostList) {
        if (host->isLoaded ()) {
            qDebug()<<Q_FUNC_INFO<<"==== here  host is loaded";
            host->unLoad ();
        }
        QObject *o = host->instance ();
        if (o) {
            IMetadataLookup *p = qobject_cast<IMetadataLookup *>(o);
            if (p) {
                qDebug()<<"   append "<<host->name ();
                mPluginList.append (p);
            } else {
                host->unLoad ();
            }
        } else {
            host->unLoad ();
        }

    }
    if (mPluginList.isEmpty ()) {
        mCurrentIndex = -1;
        mLookup = 0;
    } else {
        mLookup = mPluginList.first ();
        mCurrentIndex = 0;
    }
}

void MetadataLookupManager::initPluginNames()
{
//    if (!mPluginNameList.isEmpty ())
//        mPluginNameList.clear ();

//    mPluginNameList = mPluginLoader->getPluginNames (PluginLoader::PluginType::TypeMetadataLookup);

//    qDebug()<<Q_FUNC_INFO<<" ******* "<<mPluginNameList;

//    //因为PluginLoader默认会返回第一个插件,
//    //并且更改插件名字的时候,如果目标插件名和当前使用插件名相同,则不会发送更改的信号
//    //所以先从插件列表里面取出当前使用的插件
//    if (!mPluginNameList.isEmpty ()) {
//        if (mLookup != nullptr) {
//            mPluginNameList.removeOne (mLookup->getPluginName ());
//            if (!mPluginNameList.isEmpty ()) {
//                //指向剩余列表的第一个
//                mCurrentIndex = 0;
//            } else {
//                mCurrentIndex = -1; //重置
//            }
//        }
//    }

//    qDebug()<<Q_FUNC_INFO<<" *******  exist plugin name "<<mPluginNameList;
}

void MetadataLookupManager::processNext()
{
    qDebug()<<">>>>>>>>>>>>>>>>>>>>"<<Q_FUNC_INFO<<" queue size is "<<mWorkQueue.size ();

    if (mLookup == 0) {
        qDebug()<<"[MetadataLookupManager] Fatal error"
                  <<"we got some logic error on getting lookup plugin";
        emitFinish ();
        return;
    }

    bool ret = disconnect (mLookup, &IMetadataLookup::lookupFailed,
                        this, &MetadataLookupManager::nextLookupPlugin);

    qDebug()<<"************ "<<Q_FUNC_INFO<<" disconnect lookupFailed ret "<<ret;

    ret = disconnect (mLookup, &IMetadataLookup::lookupSucceed,
             this, &MetadataLookupManager::doLookupSucceed);

    qDebug()<<"************ "<<Q_FUNC_INFO<<" disconnect lookupSucceed ret "<<ret;

    if (mWorkQueue.isEmpty ()) {
        qDebug()<<">>>>>>>>>>>>>>>>>>>>"<<Q_FUNC_INFO<<" mWorkQueue isEmpty ";
        emitFinish ();
        return;
    }

    //将插件指向列表第一个
    mCurrentIndex = 0;
    mLookup = mPluginList.first ();

    qDebug()<<Q_FUNC_INFO<<" do work by plugin "<<mLookup->getPluginName ();

    mMutex.lock ();
    mCurrentNode = mWorkQueue.takeFirst ();
    mMutex.unlock ();
    doLookup ();
}

void MetadataLookupManager::doLookup()
{
    qDebug()<<Q_FUNC_INFO;
    if (mLookup == 0) {
        qDebug()<<"[MetadataLookupManager] Fatal error"
                  <<"we got some logic error on getting lookup plugin";
        emitFinish ();
        return;
    }
    if (mLookup->supportLookup (mCurrentNode.type)) {

        qDebug()<<"Use lookup plugin "<<mLookup->getPluginName ()
               <<" for lookup type "<<mCurrentNode.type
              <<" With song "<<mCurrentNode.data->getMeta (Common::E_FileName);

//        setLookupConnection ();
        connect (mLookup, &IMetadataLookup::lookupFailed,
                            this, &MetadataLookupManager::nextLookupPlugin);
        connect (mLookup, &IMetadataLookup::lookupSucceed,
                 this, &MetadataLookupManager::doLookupSucceed);

        mLookup->setCurrentLookupFlag (mCurrentNode.type);
        mLookup->lookup (mCurrentNode.data);
    } else {
        nextLookupPlugin ();
    }
}

void MetadataLookupManager::emitFinish()
{
    mMutex.lock ();
    mLookupStarted = false;
    mMutex.unlock ();
    emit queueFinished ();
}

void MetadataLookupManager::doLookupSucceed(const QByteArray &result)
{
    qDebug()<<Q_FUNC_INFO<<mLookup->getPluginName ()<<" lookupSucceed";

    if (mCurrentNode.data) {
        QString hash = mCurrentNode.data
                ->getMeta (Common::SongMetaTags::E_Hash).toString ();
        emit lookupSucceed (hash, result, mLookup->currentLookupFlag ());
    }
    this->processNext ();

}

void MetadataLookupManager::setLookupConnection()
{
    if (mLookup == 0) {
        qDebug()<<Q_FUNC_INFO<<" mLookup NULL";
        return;
    }

    if (mConnectList.contains (mLookup->getPluginName ())) {
        qDebug()<<Q_FUNC_INFO<<" mLookup same name";
        return;
    }
    qDebug()<<Q_FUNC_INFO<<" add connection for "<<mLookup->getPluginName ();

    mConnectList.append (mLookup->getPluginName ());

    connect (mLookup, &IMetadataLookup::lookupFailed,
             [this]{
        qDebug()<<Q_FUNC_INFO<<mLookup->getPluginName ()<<" lookupFailed try next plugin";
        this->nextLookupPlugin ();
    });

    connect (mLookup, &IMetadataLookup::lookupSucceed,
             [this](const QByteArray &result) {
        qDebug()<<Q_FUNC_INFO<<mLookup->getPluginName ()<<" lookupSucceed processNext";

        if (mCurrentNode.data) {
            QString hash = mCurrentNode.data
                    ->getMeta (Common::SongMetaTags::E_Hash).toString ();
            emit lookupSucceed (hash, result, mLookup->currentLookupFlag ());
        }
        this->processNext ();
    });
}
} //Lyrics
} //PhoenixPlayer
