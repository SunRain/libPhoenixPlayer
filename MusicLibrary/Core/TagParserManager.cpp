
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

#include "TagParserManager.h"
#include "SongMetaData.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

TagParserManager::TagParserManager(QObject *parent) : QObject(parent)
{
    mCurrentIndex = -1;
}

TagParserManager::~TagParserManager()
{
    qDebug()<<__FUNCTION__;
    emit parserQueueFinished ();

    if (!mPluginNameList.isEmpty ())
        mPluginNameList.clear ();

    qDeleteAll(mMetaList);
    if (mMetaList.isEmpty ()) {
        mMetaList.clear ();
    }
}

void TagParserManager::setPluginLoader(PluginLoader *loader)
{
    mPluginLoader = loader;
    if (mPluginLoader.isNull ())
        return;

    mPluginNameList = mPluginLoader.data ()
            ->getPluginNames (PluginLoader::PluginType::TypeMusicTagParser);

    //因为PluginLoader默认会返回第一个插件,
    //并且更改插件名字的时候,如果目标插件名和当前使用插件名相同,则不会发送更改的信号
    //所以先从插件列表里面取出当前使用的插件
    if (!mPluginNameList.isEmpty ()) {
        IMusicTagParser *parser = mPluginLoader.data ()->getCurrentMusicTagParser ();
        if (parser != nullptr) {
            mPluginList.append (parser);
            mPluginNameList.removeOne (parser->getPluginName ());
        }
    }
    if (!mPluginNameList.isEmpty ()) {
        foreach (QString str, mPluginNameList) {
            mPluginLoader.data ()
                    ->setNewPlugin (PluginLoader::PluginType::TypeMusicTagParser,
                                    str);

        }
    }
    connect (mPluginLoader.data (),
             &PluginLoader::signalPluginChanged,
             [this] (PluginLoader::PluginType type) {
        if (type == PluginLoader::PluginType::TypeMusicTagParser) {
            IMusicTagParser *parser = mPluginLoader.data ()->getCurrentMusicTagParser ();
            mPluginList.append (parser);
        }
    });

}

void TagParserManager::setPlayListDAO(IPlayListDAO *dao)
{
    mPlayListDAO = dao;
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
    }

    parserNextItem ();
}

} //MusicLibrary
} //PhoenixPlayer
