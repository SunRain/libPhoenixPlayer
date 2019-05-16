#include "MusicLibrary/LocalMusicScanner.h"

#include <QDebug>
#include <QCoreApplication>

#include "AsyncDiskLookup.h"
#include "AsyncTagParserMgrWrapper.h"
#include "PluginLoader.h"
#include "PPSettings.h"
#include "PPUtility.h"
#include "AudioMetaObject.h"
#include "SingletonPointer.h"
#include "AudioMetaObject.h"
#include "LibPhoenixPlayerMain.h"

#include "LocalMusicScannerThread.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

LocalMusicScanner::LocalMusicScanner(PPSettings *set, PluginLoader *loader, QObject *parent)
    : QObject(parent),
      m_settings(set),
      m_pluginLoader(loader)
{
    m_scanner = nullptr;
}


LocalMusicScanner::~LocalMusicScanner()
{
    if (m_scanner && m_scanner->isRunning()) {
        m_scanner->stopLookup();
        m_scanner->quit();
        m_scanner->wait (3*60*1000);
        m_scanner->deleteLater();
        m_scanner = nullptr;
    }
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
    if (dirname.isEmpty())
        m_scanner->addLookupDirs(m_settings->musicDirs());
    else
        m_scanner->addLookupDir (dirname);
    m_scanner->start (QThread::HighPriority);
}

} //MusicLibrary
} //PhoenixPlayer
