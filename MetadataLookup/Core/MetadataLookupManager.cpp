#include "MetadataLookupManager.h"

#include <QDebug>
#include <QMutex>
#include <QTimer>

#include "Common.h"
#include "MetadataLookup/IMetadataLookup.h"
#include "PluginLoader.h"
#include "SongMetaData.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer {
namespace MetadataLookup {

MetadataLookupManager::MetadataLookupManager(QObject *parent) : QObject(parent)
{
#ifdef SAILFISH_OS
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
    qDebug()<<__FUNCTION__;

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
//                o->deleteLater ();
                delete o;
                o = 0;
            }
        }
        mPluginList.clear ();
    }

//    if (!mPluginNameList.isEmpty ())
//        mPluginNameList.clear ();

//    if (mLookup != nullptr) {
//        mLookup->deleteLater ();
//    }
//    if (mBackupLookup != nullptr)
//        mBackupLookup->deleteLater ();

    qDebug()<<"after "<<__FUNCTION__;
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
    mWorkQueue.append (node);
    mMutex.unlock ();

    qDebug()<<__FUNCTION__<<" add node "<<d->getMeta (Common::E_FileName).toString ()
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
//    qDebug()<<__FUNCTION__<<" do work by plugin "<<mLookup->getPluginName ();
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
//    qDebug()<<__FUNCTION__<<" current index "<<mCurrentIndex
//           <<" with pluginList size "<<mPluginNameList.size ();

//    if (mCurrentIndex >= 0) { //如果存在其他插件
//        if (mCurrentIndex < mPluginNameList.size ()) { //防止溢出
//            qDebug()<<__FUNCTION__<<" new plugin "<<mPluginNameList.at (mCurrentIndex)
//                      <<" index "<<mCurrentIndex;

//            //指向下一个插件
//            mPluginLoader->setNewPlugin (PluginLoader::PluginType::TypeMetadataLookup,
//                                    mPluginNameList.at (mCurrentIndex++));

//        } else {
//            //所有插件都使用过了
//            QString hash = mCurrentNode.data
//                    ->getMeta (Common::SongMetaTags::E_Hash).toString ();
//            qDebug()<<__FUNCTION__<<" all plugin used ===> failed for hash "<<hash;
//            emit lookupFailed (hash, mLookup->currentLookupFlag ());

//            initPluginNames ();
//            processNext ();
//        }
//    }

    if (mPluginList.size () == 1 || mCurrentIndex == -1) { //only one plugin
        //所有插件都使用过了
        QString hash = mCurrentNode.data
                ->getMeta (Common::SongMetaTags::E_Hash).toString ();
        qDebug()<<__FUNCTION__<<" all plugin used ===> failed for hash "<<hash;
        emit lookupFailed (hash, mLookup->currentLookupFlag ());
        processNext ();
    } else {
        if (mCurrentIndex < mPluginList.size ()-1) { //防止溢出

//            bool ret = disconnect (mLookup, &IMetadataLookup::lookupFailed,
//                                this, MetadataLookupManager::nextLookupPlugin);

            bool ret = mLookup->disconnect ();
            qDebug()<<"************ "<<__FUNCTION__<<" disconnect lookupFailed ret "<<ret;

//            ret = disconnect (mLookup, &IMetadataLookup::lookupSucceed,
//                     this, MetadataLookupManager::doLookupSucceed);

//            qDebug()<<"************ "<<__FUNCTION__<<" disconnect lookupSucceed ret "<<ret;

            //指向下一个插件
            mCurrentIndex++;
            mLookup = mPluginList.at (mCurrentIndex);
            qDebug()<<__FUNCTION__<<" new plugin "<<mPluginList.at (mCurrentIndex)->getPluginName ()
                   <<" index "<<mCurrentIndex;

            if (mLookup != 0)
                doLookup ();
        } else {
            QString hash = mCurrentNode.data
                    ->getMeta (Common::SongMetaTags::E_Hash).toString ();
            qDebug()<<__FUNCTION__<<" all plugin used ===> failed for hash "<<hash;
            emit lookupFailed (hash, mLookup->currentLookupFlag ());
            processNext ();
        }
    }
}

void MetadataLookupManager::initPluginObject()
{
    qDebug()<<__FUNCTION__;

//    mLookup = mPluginLoader->getCurrentMetadataLookup ();
//    setLookupConnection ();

//    connect (mPluginLoader,
//             &PluginLoader::signalPluginChanged,
//             [this] (PluginLoader::PluginType type) {
//        if (type == PluginLoader::PluginType::TypeMetadataLookup) {
//            if (mLookup != nullptr)
//                mLookup = nullptr;
//            mLookup = mPluginLoader->getCurrentMetadataLookup ();
//            if (mLookup != nullptr) {
//                doLookup ();
//            }
//        }
//    });
    QList<QObject*> objList = mPluginLoader->getPluginList (PluginLoader::TypeMetadataLookup);
    foreach (QObject *o, objList) {
        IMetadataLookup *lookup = qobject_cast<IMetadataLookup *>(o);
        if (lookup) {
            qDebug()<<"   append "<<lookup->getPluginName ();
            mPluginList.append (lookup);
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

//    qDebug()<<__FUNCTION__<<" ******* "<<mPluginNameList;

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

//    qDebug()<<__FUNCTION__<<" *******  exist plugin name "<<mPluginNameList;
}

void MetadataLookupManager::processNext()
{
    qDebug()<<">>>>>>>>>>>>>>>>>>>>"<<__FUNCTION__<<" queue size is "<<mWorkQueue.size ();

    if (mLookup == 0) {
        qDebug()<<"[MetadataLookupManager] Fatal error"
                  <<"we got some logic error on getting lookup plugin";
        emitFinish ();
        return;
    }

    bool ret = disconnect (mLookup, &IMetadataLookup::lookupFailed,
                        this, &MetadataLookupManager::nextLookupPlugin);

    qDebug()<<"************ "<<__FUNCTION__<<" disconnect lookupFailed ret "<<ret;

    ret = disconnect (mLookup, &IMetadataLookup::lookupSucceed,
             this, &MetadataLookupManager::doLookupSucceed);

    qDebug()<<"************ "<<__FUNCTION__<<" disconnect lookupSucceed ret "<<ret;

    if (mWorkQueue.isEmpty ()) {
        qDebug()<<">>>>>>>>>>>>>>>>>>>>"<<__FUNCTION__<<" mWorkQueue isEmpty ";
        emitFinish ();
        return;
    }

//    QTimer::singleShot (200, [this] () {
//        qDebug()<<__FUNCTION__<<" do work by plugin "<<mLookup->getPluginName ();
//        //将插件指向列表第一个
//        mCurrentIndex = 0;
//        mMutex.lock ();
//        mCurrentNode = mWorkQueue.takeFirst ();
//        mMutex.unlock ();
//        doLookup ();
//    });
//    QTimer::singleShot (100, this, SLOT(slotDoLookup()));

    //将插件指向列表第一个
    mCurrentIndex = 0;
    mLookup = mPluginList.first ();

    qDebug()<<__FUNCTION__<<" do work by plugin "<<mLookup->getPluginName ();

    mMutex.lock ();
    mCurrentNode = mWorkQueue.takeFirst ();
    mMutex.unlock ();
    doLookup ();
}

void MetadataLookupManager::doLookup()
{
    qDebug()<<__FUNCTION__;
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
    qDebug()<<__FUNCTION__<<mLookup->getPluginName ()<<" lookupSucceed";

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
        qDebug()<<__FUNCTION__<<" mLookup NULL";
        return;
    }
//    if (mPreConnection.toLower () == mLookup->getPluginName ().toLower ()) {
//        qDebug()<<__FUNCTION__<<" mLookup same name";
//        return;
//    }
//    qDebug()<<__FUNCTION__<<" add connection for "<<mLookup->getPluginName ();

//    mPreConnection = mLookup->getPluginName ().toLower ();

    if (mConnectList.contains (mLookup->getPluginName ())) {
        qDebug()<<__FUNCTION__<<" mLookup same name";
        return;
    }
    qDebug()<<__FUNCTION__<<" add connection for "<<mLookup->getPluginName ();

    mConnectList.append (mLookup->getPluginName ());

    connect (mLookup, &IMetadataLookup::lookupFailed,
             [this]{
        qDebug()<<__FUNCTION__<<mLookup->getPluginName ()<<" lookupFailed try next plugin";
        this->nextLookupPlugin ();
    });

    connect (mLookup, &IMetadataLookup::lookupSucceed,
             [this](const QByteArray &result) {
        qDebug()<<__FUNCTION__<<mLookup->getPluginName ()<<" lookupSucceed processNext";

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
