#include "LocalMusicScannerInternal.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include "AudioMetaObject.h"
#include "BasePlugin.h"
#include "PluginMgr.h"
#include "PluginMgrInternal.h"

#include "DataProvider/IMusicTagParser.h"

namespace PhoenixPlayer {

FileListScanner::FileListScanner(QObject *parent)
    : QThread(parent)
{

}

FileListScanner::~FileListScanner()
{

}

void FileListScanner::addDir(const QString &dir)
{
    if (dir.isEmpty() || m_stop) {
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

void FileListScanner::stop()
{
    m_stop = true;
}

void FileListScanner::run()
{
    m_stop = false;
    QStringList fileList;
    while (true) {
        if (m_stop) {
            break;
        }
        QMutexLocker ml(&m_locker);
        if (m_dirList.isEmpty()) {
            break;
        }
        const QString path = m_dirList.takeFirst();
        ml.unlock();

        emit scanDir(path);

        QDir dir(path);
        if (!dir.exists()) {
            qDebug()<<"dir "<<path<<" not exists";
            continue;
        }
        QString canonicalPath = dir.canonicalPath();
        qDebug()<<"path ["<<path<<"] canonicalPath "<<canonicalPath;
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

AudioParser::AudioParser(QSharedPointer<PluginMgrInternal> pluginMgr, QObject *parent)
    : QThread(parent),
    m_stop(false),
    m_pluginMgrInner(pluginMgr)

{
//    foreach (const auto &it, m_pluginMgrInner->pluginMetaDataList(BasePlugin::PluginMusicTagParser)) {
//        if (it.enabled) {
//            m_tagParserList.append(it);
//        }
//    }
}

AudioParser::~AudioParser()
{
    foreach (const auto &it, m_pluginMgrInner->dataProviderList(DataProvider::IDataProvider::SupportMusicTagParser)) {
        PluginMgr::unload(it);
    }
}

void AudioParser::addFiles(const QStringList &list)
{
    if (list.isEmpty() || m_stop) {
        return;
    }
    m_mutex.lock();
    m_fileList.append(list);
    m_mutex.unlock();
}

void AudioParser::stop()
{
    m_stop = true;
}

void AudioParser::run()
{
    m_stop = false;
    AudioMetaList ll;

    while (true) {
        if (m_stop) {
            break;
        }
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
        foreach (const auto &it, m_pluginMgrInner->dataProviderList(DataProvider::IDataProvider::SupportMusicTagParser)) {
            if (!it.enabled) {
                continue;
            }
            DataProvider::IMusicTagParser *parser =
                qobject_cast<DataProvider::IMusicTagParser*>(PluginMgr::instance(it));
            if (!parser) {
                qDebug()<<"Disable invalid plugin "<<it.libraryFile;
                PluginMetaData data = it;
                data.enabled = false;
                m_pluginMgrInner->update(data);
                continue;
            }
            qDebug()<<"try to use parser ["
                   <<parser->property().name
                  <<"] to parse file ["
                 <<obj.uri()<<"]";

            if (parser->parserTag(&obj)) {
                break;
            }
        }
        emit parsed(obj);

//        foreach (auto parser, m_specParserList) {
//            auto list = parser->generate(obj);
//            if (!list.isEmpty()) {
//                m_dao->insertSpectrumData(obj, list);
//                break;
//            }
//        }
    }
    emit parsingFinished();
}


LocalMusicScannerInternal::LocalMusicScannerInternal(QSharedPointer<PluginMgrInternal> pluginMgr, QObject *parent)
    : QObject(parent),
    m_pluginMgrInner(pluginMgr)
{
    m_fileListScanner = QSharedPointer<FileListScanner>(new FileListScanner, [](FileListScanner *obj) {
        qDebug()<<"--- Delete FileListScanner";
        if (obj->isRunning()) {
            obj->stop();
            obj->quit();
            obj->wait(60*1000);
            obj->deleteLater();
        }
    });

    m_audioParser = QSharedPointer<AudioParser>(new AudioParser(pluginMgr), [](AudioParser *obj) {
        qDebug()<<"--- Delete AudioParser";
        if (obj->isRunning()) {
            obj->stop();
            obj->quit();
            obj->wait(60*1000);
            obj->deleteLater();
        }
    });
}

LocalMusicScannerInternal::~LocalMusicScannerInternal()
{
    if (m_fileListScanner->isRunning()) {
        m_fileListScanner->stop();
        m_fileListScanner->quit();
        m_fileListScanner->wait(60*1000);
    }

    if (m_audioParser->isRunning()) {
        m_audioParser->stop();
        m_audioParser->quit();
        m_audioParser->wait(60*1000);
    }
}


} //PhoenixPlayer
