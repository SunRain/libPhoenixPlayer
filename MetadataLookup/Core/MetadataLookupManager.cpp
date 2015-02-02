
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
    mCurrentIndex = -1;
    mLookup = nullptr;
    mSongMeta = nullptr;
}

MetadataLookupManager::~MetadataLookupManager()
{
    qDebug()<<__FUNCTION__;

    if (!mPluginNameList.isEmpty ())
        mPluginNameList.clear ();

    if (mSongMeta != nullptr)
        mSongMeta->deleteLater ();
}

void MetadataLookupManager::lookup(SongMetaData *data)
{
    if (mSongMeta == nullptr)
        mSongMeta = new SongMetaData(this);

    for (int i = (int)Common::SongMetaTags::E_FirstFlag + 1;
         i < (int)Common::SongMetaTags::E_LastFlag;
         ++ i) {
        mSongMeta->setMeta (Common::SongMetaTags(i),
                            data->getMeta (Common::SongMetaTags(i)));
    }
    //将插件列表指向列表第一个
    if (!mPluginNameList.isEmpty ()) {
        mCurrentIndex = 0;
    }

    if (mLookup != nullptr){
        mLookup->lookup (mSongMeta);
    }
}

void MetadataLookupManager::setPluginLoader(PluginLoader *loader)
{
    mPluginLoader = loader;
    if (mPluginLoader.isNull ())
        return;

    mPluginNameList = mPluginLoader.data ()
            ->getPluginNames (PluginLoader::PluginType::TypeMetadataLookup);

     mLookup = mPluginLoader.data ()->getCurrentLyricsLookup ();
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
            mLookup = mPluginLoader.data ()->getCurrentLyricsLookup ();
            if (mLookup != nullptr) {
                mLookup->lookup (mSongMeta);
            }
        }
    });

    connect (mLookup,
             &IMetadataLookup::lookupFailed,
             [this]{
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
    });

    connect (mLookup,
             &IMetadataLookup::lookupSucceed,
             [this](const QByteArray &result) {
        QString hash = mSongMeta->getMeta (Common::SongMetaTags::E_Hash).toString ();
        emit lookupSucceed (hash, result);
    });
}

} //Lyrics
} //PhoenixPlayer
