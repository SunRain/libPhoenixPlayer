//#include "PlayerCore/RecentPlayedQueue.h"

//#include <QStandardPaths>
//#include <QFile>
//#include <QDir>
//#include <QTextStream>
//#include <QDebug>

//#include "PPCommon.h"
//#include "AudioMetaObject.h"

//namespace PhoenixPlayer {

//using namespace MusicLibrary;

////const static char *recentFile = "recent_files.list";

//RecentPlayedQueue::RecentPlayedQueue(MusicLibraryManager *mgr, QObject *parent)
//    : MusicQueue(parent),
//      m_libraryMgr(mgr)
//{
//    queryList ();
//}

//RecentPlayedQueue::~RecentPlayedQueue()
//{

//}

//void RecentPlayedQueue::queryList()
//{
////    if (!QFile::exists(m_file)) {
////        qWarning()<<Q_FUNC_INFO<<"No recent played data found !!";
////        return;
////    }
////    QFile file(m_file);
////    if (!file.open (QIODevice::ReadOnly)) {
////        qWarning()<<Q_FUNC_INFO<<"Can't open recent played data!!";
////        return;
////    }
////    QTextStream stream(&file);
////    QStringList list = stream.readAll ().split ("\n");
////    foreach (const QString &s, list) {
////        QUrl url(s);
////        QString str;
////        if (url.isLocalFile ()) {
////            str = url.toLocalFile ();
////            if (!QFile::exists (str)) {
////                qDebug()<<Q_FUNC_INFO<<QString("file [%1] not exist").arg (str);
////                continue;
////            }
////            QFileInfo info(str);
////            AudioMetaObject o(info.absolutePath (), info.fileName (), info.size ());
////            o.setMediaType ((int)PPCommon::MediaTypeLocalFile);
////            //TODO: should fill other SongMetaData properties from music library
////            addTrack (o);
////        } else {
////            AudioMetaObject o(url);
////            o.setMediaType ((int)PPCommon::MediaTypeUrl);
////            addTrack (o);
////        }
////    }
////    file.close ();
//}

//void RecentPlayedQueue::save()
//{
//    if (QFile::exists (m_file)) {
//        QFile::remove (m_file);
//    }
//    QFile file(m_file);
//    if (!file.open (QIODevice::WriteOnly)) {
//        qWarning()<<Q_FUNC_INFO<<"Can't write recent played data!!";
//        return;
//    }
//    QTextStream s(&file);
//    QStringList list;
//    foreach (const AudioMetaObject &o, currentList ()) {
//        list.append (o.uri ().toString ());
//    }
//    s<<list.join ("\n");
//    file.close ();
//}


//} //PhoenixPlayer
