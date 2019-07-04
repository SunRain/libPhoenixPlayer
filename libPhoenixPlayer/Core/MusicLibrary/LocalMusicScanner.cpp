#include "MusicLibrary/LocalMusicScanner.h"

#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QList>

#include "AsyncDiskLookup.h"
#include "AsyncTagParserMgrWrapper.h"
#include "PluginLoader.h"
#include "PPSettings.h"
#include "PPUtility.h"
#include "AudioMetaObject.h"
#include "SingletonPointer.h"
#include "AudioMetaObject.h"
#include "LibPhoenixPlayerMain.h"
#include "MusicLibrary/MusicTagParserHost.h"
#include "MusicLibrary/IMusicTagParser.h"
#include "MusicLibrary/IMusicLibraryDAO.h"
#include "MusicLibrary/MusicLibraryDAOHost.h"
#include "MusicLibrary/SpectrumGeneratorHost.h"
#include "MusicLibrary/ISpectrumGenerator.h"

#include "LocalMusicScannerThread.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

/*
 *
 */
class FileListScanner : public QThread
{
    Q_OBJECT
public:
    explicit FileListScanner(QObject *parent = Q_NULLPTR) : QThread(parent) {}
    virtual ~FileListScanner() override {}

    void addDir(const QString &dir)
    {
        if (dir.isEmpty()) {
            return;
        }
        m_locker.lock();
        if (!m_dirList.contains(dir)) {
            QString d = dir;
            if (d.endsWith("/")) {
                d = d.mid(0, d.length()-1);
            }
            m_dirList.append(d);
        }
        m_locker.unlock();
        if (!this->isRunning()) {
            this->start(QThread::HighPriority);
        }
    }

    // QThread interface
protected:
    void run() Q_DECL_OVERRIDE
    {
        QStringList fileList;
        while (true) {
            QMutexLocker ml(&m_locker);
            if (m_dirList.isEmpty()) {
                break;
            }
            const QString path = m_dirList.takeFirst();
            ml.unlock();

            emit scanDir(path);

            QDir dir(path);
            if (!dir.exists()) {
                qDebug()<<Q_FUNC_INFO<<"dir "<<path<<" not exists";
                continue;
            }
            QString canonicalPath = dir.canonicalPath();
            qDebug()<<Q_FUNC_INFO<<" path ["<<path<<"] canonicalPath "<<canonicalPath;
            if (canonicalPath.isEmpty()) {
                continue;
            }
            dir.setPath(canonicalPath);
            dir.setFilter(QDir::Dirs | QDir::Files | /*QDir::NoSymLinks |*/ QDir::NoDotAndDotDot);
            QFileInfoList list = dir.entryInfoList ();

            foreach(const auto &info, list) {
                if (info.isDir()) {
                    this->addDir(info.absoluteFilePath());
                    continue;
                }
                QMimeType type = m_QMimeDatabase.mimeTypeForFile(info);
                //TODO 虽然建议使用inherits方法来检测,但是此处我们需要所有音频文件,
                //所以直接检测mimetype 生成的字符串
                //            if (type.inherits ("audio/mpeg")) {
                if (type.name().toLower().contains("audio")) {
                    fileList.append(QString("%1/%2").arg(path).arg(info.fileName()));
                }
            }
        }
        emit findFiles(fileList);
    }
signals:
    void scanDir(const QString &dir);
    void findFiles(const QStringList &fileList);

private:
    QStringList     m_dirList;
    QMutex          m_locker;
    QMimeDatabase   m_QMimeDatabase;
};

/***************************************************
 *
 ***************************************************/

class AudioParser : public QThread
{
    Q_OBJECT
public:
    explicit AudioParser(PPSettings *set, PluginLoader *loader, QObject *parent = Q_NULLPTR)
        : QThread(parent), m_settings(set), m_pluginLoader(loader)
    {
        QStringList tagHosts = m_settings->tagParserLibraries();
        if (tagHosts.isEmpty())
            tagHosts.append(m_pluginLoader->pluginLibraries(PPCommon::PluginMusicTagParser));

        foreach(const auto &s, tagHosts) {
            MusicTagParserHost *host = new MusicTagParserHost(s);
            if (!host->isValid()) {
                host->deleteLater();
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

        QStringList specHosts = m_settings->spectrumGeneratorLibraries();
        if (specHosts.isEmpty()) {
            specHosts.append(m_pluginLoader->pluginLibraries(PPCommon::PluginSpectrumGenerator));
        }
        foreach(const auto &s, specHosts) {
            SpectrumGeneratorHost *host = new SpectrumGeneratorHost(s);
            if (!host->isValid()) {
                host->deleteLater();
                host = nullptr;
                continue;
            }
            ISpectrumGenerator *parser = host->instance<ISpectrumGenerator>();
            if (!parser)
                continue;
            m_specHostList.append(host);
            m_specParserList.append(parser);
        }
        qDebug()<<Q_FUNC_INFO<<" m_specHostList size "<<m_tagHostList.size ();
    }
    virtual ~AudioParser() override
    {
        foreach(auto h, m_tagHostList) {
            if (!h->unLoad ())
                h->forceUnload ();
        }
        if (!m_tagHostList.isEmpty()) {
            qDeleteAll(m_tagHostList);
            m_tagHostList.clear();
        }
    }
    void addFiles(const QStringList &list)
    {
        if (list.isEmpty()) {
            return;
        }
        m_mutex.lock();
        m_fileList.append(list);
        m_mutex.unlock();
    }

    // QThread interface
protected:
    void run() Q_DECL_OVERRIDE
    {
        while (true) {
            QMutexLocker ml(&m_mutex);
            if (m_fileList.isEmpty()) {
                break;
            }
            const QString file = m_fileList.takeFirst();
            const int size = m_fileList.size();
            ml.unlock();
            if (!QFile::exists(file)) {
                continue;
            }
            emit parsingFile(file, size);

            AudioMetaObject obj(file);
            foreach (auto *parser, m_tagParserList) {
                if (parser->parserTag(&obj)) {
                    break;
                }
            }
            emit parsed(obj);

            foreach (auto parser, m_specParserList) {
                auto list = parser->generate(obj);
                if (!list.isEmpty()) {
                    emit spectrumGenerated(list);
                    break;
                }
            }
        }
        emit parsingFinished();
    }

signals:
    void parsingFile(const QString &file, int remainingSize);
    void parsed(const AudioMetaObject &obj);
    void spectrumGenerated(const QList<QList<qreal>> &list);
    void parsingFinished();


private:
    QMutex                          m_mutex;
    QStringList                     m_fileList;
    PPSettings                      *m_settings;
    PluginLoader                    *m_pluginLoader;
//    IMusicLibraryDAO                *m_dao;
    QList<MusicTagParserHost *>     m_tagHostList;
    QList<SpectrumGeneratorHost *>  m_specHostList;
    QList<IMusicTagParser *>        m_tagParserList;
    QList<ISpectrumGenerator *>     m_specParserList;
};

LocalMusicScanner::LocalMusicScanner(PPSettings *set, PluginLoader *loader, QObject *parent)
    : QObject(parent),
      m_settings(set),
      m_pluginLoader(loader)
{
//    m_scanner = nullptr;
    m_fileListScanner = new FileListScanner(this);
    m_audioParser = new AudioParser(m_settings, m_pluginLoader, this);

    connect(m_fileListScanner, &FileListScanner::scanDir,
            this, &LocalMusicScanner::searchingDir, Qt::QueuedConnection);
    connect(m_fileListScanner, &FileListScanner::findFiles,
            this, [&](const QStringList &list) {
                m_audioParser->addFiles(list);
                if (!m_audioParser->isRunning()) {
                    m_audioParser->start(QThread::HighPriority);
                }
            });

}


LocalMusicScanner::~LocalMusicScanner()
{
//    if (m_scanner && m_scanner->isRunning()) {
//        m_scanner->stopLookup();
//        m_scanner->quit();
//        m_scanner->wait (3*60*1000);
//        m_scanner->deleteLater();
//        m_scanner = nullptr;
//    }
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
    if (!m_scanner) {
        m_scanner = new LocalMusicScannerThread(m_settings, m_pluginLoader);
        connect(m_scanner, &QThread::finished,
                this, [&]() {
            m_scanner->deleteLater();
            m_scanner = nullptr;
            emit searchingFinished();
        });
    }
    if (dirname.isEmpty()) {
        m_scanner->addLookupDirs(m_settings->musicDirs());
    } else {
        m_scanner->addLookupDir(dirname);
    }
    if (!m_scanner->isRunning()) {
        m_scanner->start(QThread::HighPriority);
    }
}

} //MusicLibrary
} //PhoenixPlayer


#include "LocalMusicScanner.moc"
