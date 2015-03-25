
#include <QDebug>

#include "MetadataLookupManager.h"
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
    mBackupLookup = nullptr;
    mLookup = nullptr;
    mPreConnection = QString();
    initPlugins ();
}

MetadataLookupManager::~MetadataLookupManager()
{
    qDebug()<<__FUNCTION__;

    if (!mWorkQueue.isEmpty ()) {
        qDebug()<<"Delete work queue";
        foreach (WorkNode node, mWorkQueue) {
            if (node.data) {
                delete node.data;
                node.data = nullptr;
            }
        }
        mWorkQueue.clear ();
    }

    if (!mPluginNameList.isEmpty ())
        mPluginNameList.clear ();

    if (mLookup != nullptr) {
        mLookup->deleteLater ();
    }
    if (mBackupLookup != nullptr)
        mBackupLookup->deleteLater ();

    qDebug()<<"after "<<__FUNCTION__;
}

void MetadataLookupManager::lookup(SongMetaData *data,
                                   IMetadataLookup::LookupType type)
{
    if (mLookup == nullptr){
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
    mWorkQueue.append (node);

    qDebug()<<__FUNCTION__<<" add node "<<d->getMeta (Common::E_FileName).toString ()
              <<" list sieze "<<mWorkQueue.size ();
    if (!mLookupStarted) {
        mLookupStarted = true;
        processNext ();
    }
}

void MetadataLookupManager::nextLookupPlugin()
{
    qDebug()<<__FUNCTION__<<" current index "<<mCurrentIndex
           <<" with pluginList size "<<mPluginNameList.size ();
    if (mCurrentIndex >= 0) { //如果存在其他插件
        if (mCurrentIndex < mPluginNameList.size ()) { //防止溢出
            qDebug()<<__FUNCTION__<<" new plugin "<<mPluginNameList.at (mCurrentIndex)
                      <<" index "<<mCurrentIndex;

            //指向下一个插件
//            mCurrentIndex++; //+= 1;
//            qDebug()<<__FUNCTION__<<" current index "<<mCurrentIndex;

            mPluginLoader->setNewPlugin (PluginLoader::PluginType::TypeMetadataLookup,
                                    mPluginNameList.at (mCurrentIndex++));

        } else {
            //所有插件都使用过了
            qDebug()<<__FUNCTION__<<" all plugin used ===> failed";
            QString hash = mCurrentNode.data
                    ->getMeta (Common::SongMetaTags::E_Hash).toString ();
            emit lookupFailed (hash, mLookup->currentLookupFlag ());
            processNext ();
        }
    }
}

void MetadataLookupManager::initPlugins()
{
    qDebug()<<__FUNCTION__;
    mPluginNameList = mPluginLoader->getPluginNames (PluginLoader::PluginType::TypeMetadataLookup);
    mBackupLookup = mPluginLoader->getCurrentMetadataLookup ();
    mLookup = mBackupLookup;//mPluginLoader->getCurrentMetadataLookup ();
    setConnection (mLookup);

    //因为PluginLoader默认会返回第一个插件,
    //并且更改插件名字的时候,如果目标插件名和当前使用插件名相同,则不会发送更改的信号
    //所以先从插件列表里面取出当前使用的插件
    if (!mPluginNameList.isEmpty ()) {
        if (mLookup != nullptr) {
            mPluginNameList.removeOne (mLookup->getPluginName ());
            if (!mPluginNameList.isEmpty ()) {
                //指向剩余列表的第一个
                mCurrentIndex = 0;
            } else {
                mCurrentIndex = -1; //重置
            }
        }
    }

    connect (mPluginLoader,
             &PluginLoader::signalPluginChanged,
             [this] (PluginLoader::PluginType type) {
        if (type == PluginLoader::PluginType::TypeMetadataLookup) {
//            if (mLookup != nullptr) {
//                delete mLookup;
//                mLookup = 0;
//            }
            mLookup = mPluginLoader->getCurrentMetadataLookup ();
            if (mLookup != nullptr) {
                if (mLookup->supportLookup (mCurrentNode.type)) {

                    qDebug()<<"change lookup plugin to "<<mLookup->getPluginName ()
                           <<" for lookup type "<<mCurrentNode.type
                          <<" With song "<<mCurrentNode.data->getMeta (Common::E_FileName);

                    setConnection (mLookup);
                    mLookup->setCurrentLookupFlag (mCurrentNode.type);
                    mLookup->lookup (mCurrentNode.data);
                } else {
                    this->nextLookupPlugin ();
                }
            }
        }
    });
}

void MetadataLookupManager::processNext()
{
    qDebug()<<__FUNCTION__<<" queue size is "<<mWorkQueue.size ();

    if (mWorkQueue.isEmpty ()) {
        mLookupStarted = false;
        return;
    }
    if (mLookup == nullptr) {
        qDebug()<<"[MetadataLookupManager] Fatal error"
                  <<"we got some logic error on getting lookup plugin";
        mLookupStarted = false;
        return;
    }

    /*
     * init, backup plugin(x)
     * work A , current plugin 2(used),backup plugin(x), list 1, 2, 3
     * work A finished, start work B, at plugin 2
     * plugin 2 (used), backup plugin (x), list 1, 2, 3
     * step 1: add backup plugin to list ==> 1, 2, 3, x
     * step 2: remote used plugin from list ==> 1, 3, x
     * step 3: change backup plugin to current used plugin ==>
     *        plugin 2 (used), backup plugin (2), list 1, 3, x
     */

    //开始一个新的查询事件,初始化所有
    if (mBackupLookup->getPluginName ().toLower ()
            != mLookup->getPluginName ().toLower ()) {

        qDebug()<<__FUNCTION__<<" reset plugins";

        //如果当前使用的查询插件并不是最初的查询插件则将最初插件重新加回插件列表
        //并且同时从当前插件列表里面删除目前使用的插件名字
        if (!mPluginNameList.contains (mBackupLookup->getPluginName ())) {
            mPluginNameList.append (mBackupLookup->getPluginName ());

            //将当前插件移除,并且将备份的插件名改为当前被移除的插件
            if (mPluginNameList.contains (mLookup->getPluginName ())) {
                mPluginNameList.removeOne (mLookup->getPluginName ());
//                if (mBackupLookup != 0) {
//                    delete mBackupLookup;
//                    mBackupLookup = 0;
//                }
                mBackupLookup = mLookup;
            }
        }
    }

    qDebug()<<__FUNCTION__<<" do work by plugin "<<mLookup->getPluginName ();
    //将插件指向列表第一个
    mCurrentIndex = 0;

    mCurrentNode = mWorkQueue.takeFirst ();

    if (mLookup->supportLookup (mCurrentNode.type)) {

        qDebug()<<"Use lookup plugin "<<mLookup->getPluginName ()
               <<" for lookup type "<<mCurrentNode.type
              <<" With song "<<mCurrentNode.data->getMeta (Common::E_FileName);

        setConnection (mLookup);
        mLookup->setCurrentLookupFlag (mCurrentNode.type);
        mLookup->lookup (mCurrentNode.data);
    } else {
        nextLookupPlugin ();
    }
}

void MetadataLookupManager::setConnection(IMetadataLookup *lookup)
{
    if (lookup == nullptr) {
        qDebug()<<__FUNCTION__<<" lookup NULL";
        return;
    }
    if (mPreConnection.toLower () == lookup->getPluginName ().toLower ()) {
        qDebug()<<__FUNCTION__<<" lookup same name";
        return;
    }
    qDebug()<<__FUNCTION__<<" add connection for "<<lookup->getPluginName ();

    IMetadataLookup *look = lookup;
    mPreConnection = look->getPluginName ().toLower ();

    connect (lookup, &IMetadataLookup::lookupFailed,
             [this, &look]{
        qDebug()<<__FUNCTION__<<look->getPluginName ()<<" lookupFailed try next plugin";
        this->nextLookupPlugin ();
    });

    connect (lookup, &IMetadataLookup::lookupSucceed,
             [this, &look](const QByteArray &result) {
        qDebug()<<__FUNCTION__<<look->getPluginName ()<<" lookupSucceed processNext";

        QString hash = mCurrentNode.data
                ->getMeta (Common::SongMetaTags::E_Hash).toString ();
        emit lookupSucceed (hash, result, mLookup->currentLookupFlag ());
        processNext ();
    });
}
} //Lyrics
} //PhoenixPlayer
