#include "MusicLibrary/LocalMusicScanner.h"

#include <QDebug>
#include <QCoreApplication>

#include "AsyncDiskLookup.h"
#include "AsyncTagParserMgrWrapper.h"
#include "PluginLoader.h"
#include "Settings.h"
#include "Util.h"
#include "AudioMetaObject.h"
#include "SingletonPointer.h"
#include "AudioMetaObject.h"

#include "LocalMusicScannerThread.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

LocalMusicScanner::LocalMusicScanner(QObject *parent) :
    QObject(parent)
{
    m_scanner = nullptr;
    m_settings = Settings::instance ();
}

LocalMusicScanner::~LocalMusicScanner()
{
    if (m_scanner && m_scanner->isRunning ()) {
        m_scanner->stopLookup ();
        m_scanner->quit ();
        m_scanner->wait (3*60*1000);
        m_scanner->deleteLater ();
        m_scanner = nullptr;
    }
}

void LocalMusicScanner::scanLocalMusic()
{
    if (!m_scanner) {
        m_scanner = new LocalMusicScannerThread(0);
        connect (m_scanner, &QThread::finished,
                 [&]() {
//            m_scanner->quit ();
//            m_scanner->wait (3*60*1000);
            m_scanner->deleteLater ();
            m_scanner = nullptr;
            emit searchingFinished ();
        });
    }
    m_scanner->addLookupDirs (m_settings->musicDirs ());
    m_scanner->start (QThread::HighPriority);
}

} //MusicLibrary
} //PhoenixPlayer
