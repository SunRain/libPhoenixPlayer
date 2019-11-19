#include "MusicLibrary/LocalMusicScanner.h"

#include "Logger.h"
#include "MusicLibrary/IMusicLibraryDAO.h"

#include "private/SingletonObjectFactory.h"
#include "private/LocalMusicScannerInternal.h"
#include "private/PluginMgrInternal.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

LocalMusicScanner::LocalMusicScanner(QObject *parent)
    : QObject(parent)
{
    m_internal = SingletonObjectFactory::instance()->localMusicScannerInternal();
    m_fileListScanner = m_internal->fileListScanner();
    m_audioParser = m_internal->audioParser();

    connect(m_fileListScanner.data(), &FileListScanner::scanDir,
            this, &LocalMusicScanner::searchingDir, Qt::QueuedConnection);

    connect(m_fileListScanner.data(), &FileListScanner::findFiles,
            this, [&](const QStringList &list) {
        qDebug()<<"Find files "<<list;
        m_audioParser->addFiles(list);
        if (!m_audioParser->isRunning()) {
            m_audioParser->start(QThread::HighPriority);
        }
        emit newFileListAdded(list);
    });

    connect(m_audioParser.data(), &AudioParser::parsingFile,
            this, &LocalMusicScanner::parsingFile, Qt::QueuedConnection);

    connect(m_audioParser.data(), &AudioParser::parsed,
            this, [&](const AudioMetaObject &obj) {
        qDebug()<<"parsed file "<<obj.toJson();
        m_audioList.append(obj);
    }, Qt::QueuedConnection);

    connect(m_audioParser.data(), &AudioParser::parsingFinished,
            this, &LocalMusicScanner::insert, Qt::QueuedConnection);

}

LocalMusicScanner::~LocalMusicScanner()
{
    m_fileListScanner->disconnect(this);
    m_audioParser->disconnect(this);
}

void LocalMusicScanner::scanLocalMusic()
{
    doScann(QString());
}

void LocalMusicScanner::scanDir(const QString &dirname)
{
    if (dirname.isEmpty())
        return;
    if (dirname.startsWith("file://")) {
        QString str = dirname.mid (7);
        doScann(str);
    } else {
        doScann(dirname);
    }
}

void LocalMusicScanner::scarnDirs(const QStringList &list)
{
    foreach(const QString &dir, list) {
        scanDir(dir);
    }
}

void LocalMusicScanner::doScann(const QString &dirname)
{
    m_fileListScanner->addDir(dirname);
    if (!m_fileListScanner->isRunning()) {
        m_fileListScanner->start(QThread::HighPriority);
    }
}

void LocalMusicScanner::insert()
{
    QSharedPointer<PluginMgrInternal> mgr = SingletonObjectFactory::instance()->pluginMgrInternal();
    PluginMetaData md = mgr->usedMusicLibraryDAO();
    if (PluginMetaData::isValid(md)) {
        IMusicLibraryDAO *dao = qobject_cast<IMusicLibraryDAO*>(PluginMgr::instance(md));
        if (dao) {
            dao->beginTransaction();
            foreach (const auto &it, m_audioList) {
                dao->insertMetaData(it);
            }
            dao->commitTransaction();
            PluginMgr::unload(md);
        }
    }
     emit searchingFinished();
}

} //MusicLibrary
} //PhoenixPlayer
