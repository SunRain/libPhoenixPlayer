
#include "TagParserManager.h"

#include <QCoreApplication>
#include <QPointer>
#include <QList>
#include <QDebug>
#include <QtPlugin>
#include <QPluginLoader>
#include <QDebug>
#include <QDir>
#include <qobject.h>

#include "MusicLibrary/IMusicTagParser.h"
#include "MusicLibrary/IPlayListDAO.h"
#include "PluginLoader.h"

#include "SongMetaData.h"
#include "SingletonPointer.h"
#include "PluginHost.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

TagParserManager::TagParserManager(QObject *parent) : QObject(parent)
{
    mCurrentIndex = -1;

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    mPluginLoader = PluginLoader::instance();
#else
    mPluginLoader = SingletonPointer<PluginLoader>::instance ();
#endif
    setPluginLoader ();
    setPlayListDAO ();
}

TagParserManager::~TagParserManager()
{
    qDebug()<<__FUNCTION__;
    emit parserQueueFinished ();

    if (!mPluginHashList.isEmpty ())
        mPluginHashList.clear ();

    qDeleteAll(mMetaList);
    if (mMetaList.isEmpty ()) {
        mMetaList.clear ();
    }
}

void TagParserManager::setPluginLoader()
{
    mPluginHashList = mPluginLoader->getPluginHostHashList (Common::PluginType::PluginMusicTagParser);

    //因为PluginLoader默认会返回第一个插件,
    //并且更改插件名字的时候,如果目标插件名和当前使用插件名相同,则不会发送更改的信号
    //所以先从插件列表里面取出当前使用的插件
    if (!mPluginHashList.isEmpty ()) {
        IMusicTagParser *parser = mPluginLoader->getCurrentMusicTagParser ();
        if (parser) {
            mPluginList.append (parser);
            PluginHost *p = mPluginLoader->getCurrentPluginHost (Common::PluginMusicTagParser);
            mPluginHashList.removeOne (p->hash ());
        }
    }
    if (!mPluginHashList.isEmpty ()) {
        foreach (QString str, mPluginHashList) {
            mPluginLoader->setNewPlugin (Common::PluginType::PluginMusicTagParser, str);
        }
    }
    connect (mPluginLoader,
             &PluginLoader::signalPluginChanged,
             [this] (Common::PluginType type) {
        if (type == Common::PluginType::PluginMusicTagParser) {
            IMusicTagParser *parser = mPluginLoader->getCurrentMusicTagParser ();
            mPluginList.append (parser);
        }
    });

}

void TagParserManager::setPlayListDAO()
{
    mPlayListDAO = mPluginLoader->getCurrentPlayListDAO ();
}

void TagParserManager::addItem(SongMetaData *data, bool startImmediately)
{
    SongMetaData *d = data ;
    mMetaList.append (d);
    if (startImmediately)
        startParserLoop ();
}

bool TagParserManager::startParserLoop()
{
    emit parserPending ();

    if (mMetaList.isEmpty ()) {
        qDebug()<<"No Song meta";
        emit parserQueueFinished ();
        return false;
    }

    SongMetaData *data = mMetaList.takeFirst ();
    parserItem (data);
    return true;
}

void TagParserManager::parserNextItem()
{
    if (mMetaList.isEmpty ()) {
        qDebug()<<"Parser finished or meta list is empty";
        emit parserQueueFinished ();
        return;
    }
    SongMetaData *data = mMetaList.takeFirst ();
    parserItem (data);
}

void TagParserManager::parserItem(SongMetaData *data)
{
    if (data == 0) {
        return;
    }
    //如果没有解析插件,则直接将相关的meta信息存储到数据库
    if (!mPluginList.isEmpty ()) {
        //开始解析
        foreach (IMusicTagParser *parser, mPluginList) {
            if (parser->parserTag (data)) {
                break;
            }
        }
    }
    if (!mPlayListDAO.isNull ()) {
        mPlayListDAO.data ()->insertMetaData (data);
        data->deleteLater ();
    } else {
        qDebug()<<"Warning: "<<__FUNCTION__<<" mPlayListDAO is null";
    }

    parserNextItem ();
}

} //MusicLibrary
} //PhoenixPlayer
