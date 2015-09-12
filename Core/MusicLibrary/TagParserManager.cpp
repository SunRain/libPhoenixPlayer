
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
    m_currentIndex = -1;

    m_pluginLoader = PluginLoader::instance ();

    setPluginLoader ();
    setPlayListDAO ();
}

TagParserManager::~TagParserManager()
{
    qDebug()<<Q_FUNC_INFO;
    emit parserQueueFinished ();

    if (!m_pluginHashList.isEmpty ())
        m_pluginHashList.clear ();

    if (m_metaList.isEmpty ()) {
        qDeleteAll(m_metaList);
        m_metaList.clear ();
    }
}

void TagParserManager::setPluginLoader()
{
    m_pluginHashList = m_pluginLoader->getPluginHostHashList (Common::PluginType::PluginMusicTagParser);

    //因为PluginLoader默认会返回第一个插件,
    //并且更改插件名字的时候,如果目标插件名和当前使用插件名相同,则不会发送更改的信号
    //所以先从插件列表里面取出当前使用的插件
    if (!m_pluginHashList.isEmpty ()) {
        IMusicTagParser *parser = m_pluginLoader->getCurrentMusicTagParser ();
        if (parser) {
            m_pluginList.append (parser);
            PluginHost *p = m_pluginLoader->getCurrentPluginHost (Common::PluginMusicTagParser);
            m_pluginHashList.removeOne (p->hash ());
        }
    }
    if (!m_pluginHashList.isEmpty ()) {
        foreach (QString str, m_pluginHashList) {
            m_pluginLoader->setNewPlugin (Common::PluginType::PluginMusicTagParser, str);
        }
    }
    connect (m_pluginLoader,
             &PluginLoader::signalPluginChanged,
             [&] (Common::PluginType type) {
        if (type == Common::PluginType::PluginMusicTagParser) {
            IMusicTagParser *parser = m_pluginLoader->getCurrentMusicTagParser ();
            m_pluginList.append (parser);
        }
    });

}

void TagParserManager::setPlayListDAO()
{
    m_playListDAO = m_pluginLoader->getCurrentPlayListDAO ();
}

void TagParserManager::addItem(SongMetaData *data, bool startImmediately)
{
    qDebug()<<Q_FUNC_INFO <<" item info "<<data->toString ();

    m_mutex.lock ();
    SongMetaData *d = new SongMetaData(data, 0);
    m_metaList.append (d);
    m_mutex.unlock ();

    if (startImmediately)
        startParserLoop ();
}

//void TagParserManager::parserImmediately(const QList<SongMetaData *> &list)
//{
//    this->parserImmediately (list);
//}

void TagParserManager::parserImmediately(QList<SongMetaData *> *list)
{
    qDebug()<<Q_FUNC_INFO;
    emit parserPending ();
    for (int i=0; i<list->size (); ++i) {
        //如果没有解析插件,则直接将相关的meta信息存储到数据库
        if (!m_pluginList.isEmpty ()) {
            //开始解析
            foreach (IMusicTagParser *parser, m_pluginList) {
                if (parser->parserTag (list->at (i))) {
                    break;
                }
            }
        }
    }
    emit parserQueueFinished ();
}

bool TagParserManager::startParserLoop()
{
    emit parserPending ();

    if (m_metaList.isEmpty ()) {
        qDebug()<<"No Song meta";
        emit parserQueueFinished ();
        return false;
    }
    m_mutex.lock ();
    SongMetaData *d = nullptr;
    if (!m_metaList.isEmpty ())
        d = m_metaList.takeFirst ();
    m_mutex.unlock ();
    if (d)
        parserItem (d);
    return true;
}

void TagParserManager::parserNextItem()
{
    if (m_metaList.isEmpty ()) {
        qDebug()<<"Parser finished or meta list is empty";
        emit parserQueueFinished ();
        return;
    }
    m_mutex.lock ();
    SongMetaData *data = m_metaList.takeFirst ();
    m_mutex.unlock ();
    parserItem (data);
}

void TagParserManager::parserItem(SongMetaData *data)
{
    if (!data) {
        return;
    }
    //如果没有解析插件,则直接将相关的meta信息存储到数据库
    if (!m_pluginList.isEmpty ()) {
        //开始解析
        foreach (IMusicTagParser *parser, m_pluginList) {
            if (parser->parserTag (data)) {
                break;
            }
        }
    }
    if (!m_playListDAO.isNull ()) {
        m_playListDAO.data ()->insertMetaData (data);
        data->deleteLater ();
        data = nullptr;
    } else {
        qWarning()<<Q_FUNC_INFO<<" mPlayListDAO is null";
    }

    parserNextItem ();
}

} //MusicLibrary
} //PhoenixPlayer
