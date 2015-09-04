
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

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    mPluginLoader = PluginLoader::instance();
#else
    m_pluginLoader = SingletonPointer<PluginLoader>::instance ();
#endif
    setPluginLoader ();
    setPlayListDAO ();
}

TagParserManager::~TagParserManager()
{
    qDebug()<<Q_FUNC_INFO;
    emit parserQueueFinished ();

    if (!m_pluginHashList.isEmpty ())
        m_pluginHashList.clear ();

    qDeleteAll(m_metaList);
    if (m_metaList.isEmpty ()) {
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

    SongMetaData *d = data ;
    m_metaList.append (d);
    if (startImmediately)
        startParserLoop ();
}

void TagParserManager::parserImmediately(const QList<SongMetaData *> &list)
{
    this->parserImmediately (list);
}

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

    SongMetaData *data = m_metaList.takeFirst ();
    parserItem (data);
    return true;
}

void TagParserManager::parserNextItem()
{
    if (m_metaList.isEmpty ()) {
        qDebug()<<"Parser finished or meta list is empty";
        emit parserQueueFinished ();
        return;
    }
    SongMetaData *data = m_metaList.takeFirst ();
    parserItem (data);
}

void TagParserManager::parserItem(SongMetaData *data)
{
    if (data == 0) {
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
    } else {
        qWarning()<<Q_FUNC_INFO<<" mPlayListDAO is null";
    }

    parserNextItem ();
}

} //MusicLibrary
} //PhoenixPlayer
