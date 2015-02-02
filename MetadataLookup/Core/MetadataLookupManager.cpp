
#include <QDebug>

#include "MetadataLookupManager.h"
#include "Common.h"
#include "MetadataLookup/IMetadataLookup.h"
#include "PluginLoader.h"
#include "SongMetaData.h"

namespace PhoenixPlayer {
namespace MetadataLookup {

MetadataLookupManager::MetadataLookupManager(QObject *parent) : QObject(parent)
{
    //reset ();
    initPlugins ();

    qDebug()<<__FUNCTION__<<" after ";

    connect (mLookup,
             &IMetadataLookup::lookupFailed,
             [this]{
        this->nextLookupPlugin ();
    });

    connect (mLookup,
             &IMetadataLookup::lookupSucceed,
             [this](const QByteArray &result) {
        QString hash = mCurrentNode.data
                ->getMeta (Common::SongMetaTags::E_Hash).toString ();
        emit lookupSucceed (hash, result, mLookup->currentLookupFlag ());
    });
}

MetadataLookupManager::~MetadataLookupManager()
{
    qDebug()<<__FUNCTION__;

    if (!mWorkQueue.isEmpty ()) {
        qDebug()<<"Delete work queue";
        foreach (WorkNode node, mWorkQueue) {
            if (node.data)
                node.data->deleteLater ();
        }
        mWorkQueue.clear ();
    }
    if (mCurrentNode.data)
        mCurrentNode.data->deleteLater ();

    if (!mPluginNameList.isEmpty ())
        mPluginNameList.clear ();

    if (mLookup)
        mLookup->deleteLater ();

    if (mBackupLookup)
        mBackupLookup->deleteLater ();

}

void MetadataLookupManager::reset()
{
//    mCurrentIndex = -1;

//    if (mLookup != nullptr)
//        mLookup->deleteLater ();
//    mLookup = nullptr;

//    if (mSongMeta != nullptr)
//        mSongMeta->deleteLater ();
//    mSongMeta = nullptr;

//    mCurLookupType = IMetadataLookup::LookupType::TypeUndefined;

//    if (!mPluginNameList.isEmpty ())
//        mPluginNameList.clear ();

//    initPlugins ();
}

void MetadataLookupManager::lookup(SongMetaData *data,
                                   IMetadataLookup::LookupType type)
{
    qDebug()<<__FUNCTION__;

    if (mLookup == nullptr){
        qDebug()<<"MetadataLookupManager we can't found any lookup plugin now";
        emit lookupFailed ();
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

    processNext ();
}

void MetadataLookupManager::setPluginLoader(PluginLoader *loader)
{
    mPluginLoader = loader;

    initPlugins ();
}

void MetadataLookupManager::nextLookupPlugin()
{
    qDebug()<<__FUNCTION__;

    if (mCurrentIndex >= 0) { //如果存在其他插件
        if (!mPluginLoader.isNull ()) {
            if (mCurrentIndex < mPluginNameList.size ()) { //防止溢出
                mPluginLoader.data ()
                        ->setNewPlugin (PluginLoader::PluginType::TypeMetadataLookup,
                                        mPluginNameList.at (mCurrentIndex));
                //指向下一个插件
                mCurrentIndex += 1;
            } else {
                //所有插件都使用过了
                emit lookupFailed ();
            }
        }
    }
}

void MetadataLookupManager::initPlugins()
{
        qDebug()<<__FUNCTION__;

    if (mPluginLoader.isNull ())
        return;

    qDebug()<<__FUNCTION__<<" ===  2";

    mPluginNameList = mPluginLoader.data ()
            ->getPluginNames (PluginLoader::PluginType::TypeMetadataLookup);

     mLookup = mPluginLoader.data ()->getCurrentMetadataLookup ();
     mBackupLookup = mPluginLoader.data ()->getCurrentMetadataLookup ();
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

    connect (mPluginLoader.data (),
             &PluginLoader::signalPluginChanged,
             [this] (PluginLoader::PluginType type) {
        if (type == PluginLoader::PluginType::TypeMetadataLookup) {
            mLookup = mPluginLoader.data ()->getCurrentMetadataLookup ();
            if (mLookup != nullptr) {
                if (mLookup->supportLookup (mCurrentNode.type)) {
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
    qDebug()<<__FUNCTION__;

    if (mWorkQueue.isEmpty ())
        return;

    if (!mLookup) {
        qDebug()<<"[MetadataLookupManager] Fatal error"
                  <<"we got some logic error on getting lookup plugin";
        return;
    }

    qDebug()<<"MetadataLookupManager start processNext";

    //队列里面下一个的查询内容,初始化所有
    if (mBackupLookup->getPluginName ().toLower ()
            != mLookup->getPluginName ().toLower ()) {
        //如果当前使用的查询插件并不是最初的查询插件
        if (mPluginNameList.contains (mBackupLookup->getPluginName ()))
            mPluginNameList.append (mBackupLookup->getPluginName ());
    }
    //将插件指向列表第一个
    mCurrentIndex = 0;

    WorkNode mCurrentNode = mWorkQueue.takeFirst ();

    if (mLookup->supportLookup (mCurrentNode.type)) {
        mLookup->setCurrentLookupFlag (mCurrentNode.type);
        mLookup->lookup (mCurrentNode.data);
    } else {
        nextLookupPlugin ();
    }
}

} //Lyrics
} //PhoenixPlayer
