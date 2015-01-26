
#include <QCoreApplication>
#include <QPointer>
#include <QList>
#include <QDebug>
#include <QtPlugin>
#include <QPluginLoader>
#include <QDebug>
#include <QDir>
#include <qobject.h>


#include "TagParserManager.h"
#include "IMusicTagParser.h"
#include "SongMetaData.h"
#include "IPlayListDAO.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

TagParserManager::TagParserManager(QObject *parent) : QObject(parent)
{
    //TODO: 根据系统来设置插件的默认路径
    mPluginPath = QString("%1/plugins").arg(QCoreApplication::applicationDirPath ());
    initPlugin ();
}

TagParserManager::~TagParserManager()
{
    qDebug()<<__FUNCTION__;
    emit parserQueueFinished ();

    if (!mPluginList.isEmpty ()) {
        //删除List里面的指针类型文件
        qDeleteAll(mPluginList);
        mPluginList.clear ();
    }
    qDeleteAll(mMetaList);
    if (mMetaList.isEmpty ()) {
        mMetaList.clear ();
    }
}

void TagParserManager::setPluginPath(const QString &path)
{
    if (!path.isEmpty ()) {
        mPluginPath = path;
        initPlugin ();
    }
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

void TagParserManager::initPlugin()
{
    mPluginList.clear ();

    //system plugins
    foreach (QObject *plugin, QPluginLoader::staticInstances()) {
        if (plugin) {
            IMusicTagParser *interface = qobject_cast<IMusicTagParser*>(plugin);
            if (interface) {
                mPluginList.append(interface);
            }
        }
    }

    // dynamic plugins
    QDir dir(mPluginPath);
    foreach (QString fileName, dir.entryList(QDir::Files)) {
        QPluginLoader loader(dir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            IMusicTagParser *interface = qobject_cast<IMusicTagParser*>(plugin);
            if (interface) {
                mPluginList.append(interface);
            } else {
                qDebug()<<"cant qobject_cast for "<<dir.absoluteFilePath(fileName);
            }
        } else {
            qDebug()<<"no plugin for "<<dir.absoluteFilePath(fileName);
        }
    }
}

void TagParserManager::parserNextItem()
{
    if (mMetaList.isEmpty ()) {
        qDebug()<<"mMetaList.isEmpty";
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
