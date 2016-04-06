#include "LocalMusicScannerThread.h"

#include <QDir>
#include <QFileInfo>
#include <QMimeType>
#include <QStandardPaths>

#include "AudioMetaObject.h"
#include "PluginLoader.h"
#include "Settings.h"

#include "MusicLibrary/MusicTagParserHost.h"
#include "MusicLibrary/IMusicTagParser.h"
#include "MusicLibrary/IMusicLibraryDAO.h"
#include "MusicLibrary/MusicLibraryDAOHost.h"

namespace PhoenixPlayer {
namespace MusicLibrary {
LocalMusicScannerThread::LocalMusicScannerThread(QObject *parent) :
    QThread(parent)
{
    m_pluginLoader = PluginLoader::instance ();
    m_settings = Settings::instance ();
    m_dao = nullptr;
    MusicLibraryDAOHost *dh = m_pluginLoader->curDAOHost ();
    if (dh)
        m_dao = dh->instance<IMusicLibraryDAO>();

    QStringList tagHosts = m_settings->tagPaserLibraries ();
    if (tagHosts.isEmpty ())
        tagHosts.append (m_pluginLoader->pluginLibraries (Common::PluginMusicTagParser));

    foreach (QString s, tagHosts) {
        MusicTagParserHost *host = new MusicTagParserHost(s);
        if (!host->isValid ()) {
            host->deleteLater ();
            host = nullptr;
            continue;
        }
        IMusicTagParser *parser = host->instance<IMusicTagParser>();
        if (!parser)
            continue;
        m_tagHostList.append (host);
        m_tagParserList.append (parser);
    }
    qDebug()<<Q_FUNC_INFO<<" m_tagParserList size "<<m_tagHostList.size ();
}

LocalMusicScannerThread::~LocalMusicScannerThread()
{
    foreach (IMusicTagParser *p, m_tagParserList) {
        p = nullptr;
    }
    foreach (MusicTagParserHost *h, m_tagHostList) {
        if (!h->unLoad ())
            h->forceUnload ();
    }
    if (!m_tagHostList.isEmpty ()) {
        qDeleteAll(m_tagHostList);
        m_tagHostList.clear ();
    }
    if (!m_tagParserList.isEmpty ())
        m_tagParserList.clear ();

    if (!m_pathList.isEmpty ())
        m_pathList.clear ();
}

void LocalMusicScannerThread::stopLookup()
{
    m_stopLookupFlag = true;
}

void LocalMusicScannerThread::addLookupDir(const QString &dirName, bool lookupImmediately)
{
    m_mutex.lock ();
    if (!m_pathList.contains (dirName))
        m_pathList.append (dirName);
    m_mutex.unlock ();
    if (lookupImmediately) {
        this->start ();
    }
}

void LocalMusicScannerThread::addLookupDirs(const QStringList &dirList, bool lookupImmediately)
{
    m_mutex.lock ();
    m_pathList.append (dirList);
    m_pathList.removeDuplicates ();
    m_mutex.unlock ();
    if (lookupImmediately) {
        this->start ();
    }
}

void LocalMusicScannerThread::run()
{
    qDebug()<<Q_FUNC_INFO<<m_pathList;
    m_mutex.lock ();
    if (m_pathList.isEmpty ()) {
        QString tmp = QString("%1/%2").arg (QDir::homePath ())
                .arg(QStandardPaths::displayName (QStandardPaths::MusicLocation));

        qDebug()<<"Lookup default dir "<<tmp;
        m_pathList.append (tmp);
    }
    m_mutex.unlock ();
    if (m_dao)
        m_dao->beginTransaction ();
    while (!m_pathList.isEmpty ()) {
        m_mutex.lock ();
        if (m_stopLookupFlag) {
            m_pathList.clear ();
            m_mutex.unlock ();
            break;
        }
        QString p = m_pathList.takeFirst ();
        m_mutex.unlock ();
        scanDir (p);
    }
    if (m_dao)
        m_dao->commitTransaction ();
}

void LocalMusicScannerThread::scanDir(const QString &path)
{
    if (m_stopLookupFlag)
        return;

    QDir dir(path);
    if (!dir.exists ())
        return;

    dir.setFilter (QDir::Dirs | QDir::Files | /*QDir::NoSymLinks |*/ QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList ();
    qDebug()<<Q_FUNC_INFO<<QString("scanDir [%1]").arg (path)<<" list "<<dir.entryList ();
    foreach (QFileInfo info, list) {
        if (info.isDir ()) {
            m_mutex.lock ();
            qDebug()<<Q_FUNC_INFO<<QString("current item [%1] is dir, add [%2] to list")
                      .arg (info.baseName ()).arg (info.absoluteFilePath ());
            m_pathList.append (info.absoluteFilePath ());
            m_mutex.unlock ();
        } else {
            QMimeType type = m_QMimeDatabase.mimeTypeForFile (info);

            //TODO 虽然建议使用inherits方法来检测,但是此处我们需要所有音频文件,
            //所以直接检测mimetype 生成的字符串
//            if (type.inherits ("audio/mpeg")) {
            if (type.name ().contains ("audio") || type.name ().contains ("Audio")) {
//                AudioMetaObject *data = new AudioMetaObject(path, info.fileName (), info.size (), 0);
                AudioMetaObject data(path, info.fileName (), info.size ());
//                foreach (MusicTagParserHost *host, m_tagHostList) {
                foreach (IMusicTagParser *parser, m_tagParserList) {
                    if (m_stopLookupFlag)
                        break;
//                    IMusicTagParser *parser = host->instance<IMusicTagParser>();
//                    if (!parser)
//                        continue;
                    if (parser->parserTag (&data)) {
                        break;
                    }
                }
                qDebug()<<Q_FUNC_INFO<<"SongMetaData values ["<<data.toJson ()<<"]";
                if (m_dao)
                    m_dao->insertMetaData (data);
            }
        }
    }
}

} //MusicLibrary
} //PhoenixPlayer
