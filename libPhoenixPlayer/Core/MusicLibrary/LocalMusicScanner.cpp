#include "MusicLibrary/LocalMusicScanner.h"

#include <QDebug>
#include <QCoreApplication>

#include "AsyncDiskLookup.h"
#include "AsyncTagParserMgrWrapper.h"
#include "PluginLoader.h"
#include "Settings.h"
#include "Util.h"
#include "SongMetaData.h"
#include "SingletonPointer.h"
#include "SongMetaData.h"

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
//    if (!m_tagParserWrapper) {
//        m_tagParserWrapper = new AsyncTagParserMgrWrapper(this);
//        connect (m_tagParserWrapper, &AsyncTagParserMgrWrapper::started, [&] {
//            qDebug()<<Q_FUNC_INFO<<" mTagParserWrapper started";
//        });

//        connect (m_tagParserWrapper, &AsyncTagParserMgrWrapper::finished, [&] {
//            qDebug()<<Q_FUNC_INFO<<" mTagParserWrapper finished";

//            m_dao->beginTransaction ();
//            for (int i=0; i<m_metaDataList.size (); ++i) {
//                m_dao->insertMetaData (m_metaDataList.at (i));
//            }
//            m_dao->commitTransaction ();

//            //delete all items in the list
//            if (!m_metaDataList.isEmpty ()) {
//                qDeleteAll(m_metaDataList);
//                m_metaDataList.clear ();
//            }

//            m_tagParserWrapper->deleteLater ();
//            m_tagParserWrapper = nullptr;

//            emit searchingFinished ();
//        });
//    }

//    if (!m_asyncDiskLookup) {
//        m_asyncDiskLookup = new AsyncDiskLookup(this);

//        connect (m_asyncDiskLookup, &AsyncDiskLookup::started, [&] {
//            qDebug()<<Q_FUNC_INFO<<" AsyncDiskLookup::started";
//        });

//        connect (m_asyncDiskLookup, &AsyncDiskLookup::finished,
//                 [&] (const QList<SongMetaData *> &list){
//            qDebug()<<Q_FUNC_INFO<<" AsyncDiskLookup::finished , result list size "<<list.size();
//            foreach (SongMetaData *d, list) {
////                SongMetaData *data = new SongMetaData(0);
////                for (int i = (int)(Common::SongMetaTags::E_FirstFlag) +1;
////                     i < (int)Common::SongMetaTags::E_LastFlag;
////                     ++i) {
////                    data->setMeta (Common::SongMetaTags(i), d->getMeta (Common::SongMetaTags(i)));
////                }
//                SongMetaData *data = new SongMetaData(d, 0);
//                m_metaDataList.append (data);
//            }

//            m_asyncDiskLookup->deleteLater ();
//            m_asyncDiskLookup = nullptr;
//            m_tagParserWrapper->parser (&m_metaDataList);
//        });
//    }

//    if (m_tagParserWrapper && m_tagParserWrapper->isRunning ()) {
//        qWarning()<<Q_FUNC_INFO<<" Parser track tags, can't scan local music atm";
//        return;
//    }

//    m_asyncDiskLookup->setLookupDirs (m_settings->getMusicDirs ());
//    m_asyncDiskLookup->startLookup ();
    if (!m_scanner) {
        m_scanner = new LocalMusicScannerThread(0);
        connect (m_scanner, &QThread::finished,
                 [&]() {
            m_scanner->quit ();
            m_scanner->wait (3*60*1000);
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
