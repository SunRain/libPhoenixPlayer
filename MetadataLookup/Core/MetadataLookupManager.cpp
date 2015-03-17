
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

    if (!mLookup.isNull ()) {
        delete mLookup.data ();
        mLookup.clear ();
    }
    qDebug()<<"after "<<__FUNCTION__;
}

//void MetadataLookupManager::reset()
//{
//}

void MetadataLookupManager::lookup(SongMetaData *data,
                                   IMetadataLookup::LookupType type)
{
    if (mLookup.isNull ()){
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

    processNext ();
}

//void MetadataLookupManager::setPluginLoader(PluginLoader *loader)
//{
//    mPluginLoader = loader;

//    initPlugins ();
//}

void MetadataLookupManager::nextLookupPlugin()
{
    if (mCurrentIndex >= 0) { //如果存在其他插件
        if (mCurrentIndex < mPluginNameList.size ()) { //防止溢出
            mPluginLoader->setNewPlugin (PluginLoader::PluginType::TypeMetadataLookup,
                                    mPluginNameList.at (mCurrentIndex));
            //指向下一个插件
            mCurrentIndex += 1;
        } else {
            //所有插件都使用过了
//            emit lookupFailed ();
            QString hash = mCurrentNode.data
                    ->getMeta (Common::SongMetaTags::E_Hash).toString ();
            emit lookupFailed (hash, mLookup.data ()->currentLookupFlag ());
        }
    }
}

void MetadataLookupManager::initPlugins()
{
    mPluginNameList = mPluginLoader->getPluginNames (PluginLoader::PluginType::TypeMetadataLookup);

     mLookup = mPluginLoader->getCurrentMetadataLookup ();
     mBackupLookup = mPluginLoader->getCurrentMetadataLookup ();
    //因为PluginLoader默认会返回第一个插件,
    //并且更改插件名字的时候,如果目标插件名和当前使用插件名相同,则不会发送更改的信号
    //所以先从插件列表里面取出当前使用的插件
    if (!mPluginNameList.isEmpty ()) {
        if (!mLookup.isNull ()) {
            mPluginNameList.removeOne (mLookup.data ()->getPluginName ());
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
            mLookup = mPluginLoader->getCurrentMetadataLookup ();
            if (!mLookup.isNull ()) {
                if (mLookup.data ()->supportLookup (mCurrentNode.type)) {

                    qDebug()<<"change lookup plugin to "<<mLookup.data ()->getPluginName ()
                           <<" for lookup type "<<mCurrentNode.type
                          <<" With song "<<mCurrentNode.data->getMeta (Common::E_FileName);

                    mLookup.data ()->setCurrentLookupFlag (mCurrentNode.type);
                    mLookup.data ()->lookup (mCurrentNode.data);
                } else {
                    this->nextLookupPlugin ();
                }
            }
        }
    });

    connect (mLookup.data (),
             &IMetadataLookup::lookupFailed,
             [this]{
        this->nextLookupPlugin ();
    });

    connect (mLookup.data (),
             &IMetadataLookup::lookupSucceed,
             [this](const QByteArray &result) {
        QString hash = mCurrentNode.data
                ->getMeta (Common::SongMetaTags::E_Hash).toString ();
        emit lookupSucceed (hash, result, mLookup.data ()->currentLookupFlag ());
    });
}

void MetadataLookupManager::processNext()
{
    if (mWorkQueue.isEmpty ())
        return;

    if (mLookup.isNull ()) {
        qDebug()<<"[MetadataLookupManager] Fatal error"
                  <<"we got some logic error on getting lookup plugin";
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
            != mLookup.data ()->getPluginName ().toLower ()) {
        //如果当前使用的查询插件并不是最初的查询插件则将最初插件重新加回插件列表
        //并且同时从当前插件列表里面删除目前使用的插件名字
        if (!mPluginNameList.contains (mBackupLookup->getPluginName ())) {
            mPluginNameList.append (mBackupLookup->getPluginName ());

            //将当前插件移除,并且将备份的插件名改为当前被移除的插件
            if (mPluginNameList.contains (mLookup.data ()->getPluginName ())) {
                mPluginNameList.removeOne (mLookup.data ()->getPluginName ());
                mBackupLookup = mLookup;
            }
        }
    }

    //将插件指向列表第一个
    mCurrentIndex = 0;

    mCurrentNode = mWorkQueue.takeFirst ();

    if (mLookup.data ()->supportLookup (mCurrentNode.type)) {

        qDebug()<<"Use lookup plugin "<<mLookup.data ()->getPluginName ()
               <<" for lookup type "<<mCurrentNode.type
              <<" With song "<<mCurrentNode.data->getMeta (Common::E_FileName);

        mLookup.data ()->setCurrentLookupFlag (mCurrentNode.type);
        mLookup.data ()->lookup (mCurrentNode.data);
    } else {
        nextLookupPlugin ();
    }
}

} //Lyrics
} //PhoenixPlayer
