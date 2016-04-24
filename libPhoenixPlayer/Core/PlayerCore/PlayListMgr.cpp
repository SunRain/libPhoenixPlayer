#include "PlayerCore/PlayListMgr.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>

#include <QtMultimedia/QMediaPlaylist>

#include "PluginLoader.h"
#include "MusicLibrary/IMusicLibraryDAO.h"

#include "Settings.h"
#include "AudioMetaObject.h"

#include "PlayListFormat.h"
#include "M3uPlayListFormat.h"

namespace PhoenixPlayer {

PlayListMgr::PlayListMgr(Settings *set, QObject *parent)
    : MusicQueue(parent)
//    , m_random(false)
//    , m_currentIndex(-1)
//    , m_count(0)
    , m_settings(set)
{
    sizeLimit (-1);
    skipDuplicates (false);

    //TODO support different playlist format
    m_listFormat = new M3uPlayListFormat(this);
    m_playListDir = m_settings->playListDir ();

    connect (m_settings, &Settings::playListDirChanged,
             [&](QString arg) {
        m_playListDir = arg;
        queryPlayLists ();
    });
    queryPlayLists ();
}

PlayListMgr::~PlayListMgr()
{
//    if (!m_trackList.isEmpty ()) {
//        m_trackList.clear ();
//    }
}

//void PlayListMgr::addTrack(const AudioMetaObject &song)
//{
//    if (song.isEmpty ())
//        return
//    m_trackList.append (song);
//}

//void PlayListMgr::addTrack(const AudioMetaList &list)
//{
//    if (list.isEmpty ())
//        return;
//    m_trackList.append (list);
////    emit trackListAdded (list);
//}

//void PlayListMgr::removeTrack(const int &index)
//{
//    if (index < 0 || m_trackList.isEmpty () || index >= m_trackList.size ()) {
//        qDebug()<<Q_FUNC_INFO<<"invalid index";
//        return;
//    }
//    //FIXME is it right to use QList.removeAt(i) function?
////    QList<AudioMetaObject *> tmp;
////    AudioMetaList tmp;
////    for (int i=0; i<m_trackList.size (); ++i) {
////        if (i == index) {
////            AudioMetaObject *d = m_trackList.value (i);
////            d->deleteLater ();
////            d = nullptr;
////            continue;
////        }
////        tmp.append (m_trackList.value (i));
////    }
////    m_trackList.clear ();
////    foreach (AudioMetaObject *d, tmp) {
////        m_trackList.append (d);
////    }
////    int pos = -1;
////    for (int i=0; i<m_trackList.size (); ++i) {
////        if (index == i) {
////            pos = i;
////            break;
////        }
////    }
////    if (pos < 0)
////        return;
//    m_trackList.removeAt (index);
////    emit trackRemoved (index);
//}

//void PlayListMgr::removeTracks(const int &startPos, const int &endPos)
//{
//    if (startPos < 0 || endPos < 0 || startPos > endPos
//            || m_trackList.isEmpty ()
//            || startPos >= m_trackList.size () || endPos >= m_trackList.size ()) {
//        qDebug()<<Q_FUNC_INFO<<"invalid pos";
//        return false;
//    }

//    //FIXME is it right to use QList.removeAt(i) function?
////    QList<AudioMetaObject *> tmp;
//    AudioMetaList tmp;
//    for (int i=0; i<m_trackList.size (); ++i) {
//        if (i >= startPos && i <= endPos) {
////            AudioMetaObject *d = m_trackList.value (i);
////            d->deleteLater ();
////            d = nullptr;
//            continue;
//        }
//        tmp.append (m_trackList.value (i));
//    }
//    m_trackList.clear ();
////    foreach (AudioMetaObject d, tmp) {
////        m_trackList.append (d);
////    }
//    m_trackList.append (tmp);
//}

//void PlayListMgr::clear()
//{
//    if (!m_trackList.isEmpty ()) {
////        qDeleteAll(m_trackList);
//        m_trackList.clear ();
//    }
//    m_currentIndex = -1;
//}

//bool PlayListMgr::isEmpty()
//{
//    return m_trackList.isEmpty ();
//}

//int PlayListMgr::currentIndex() const
//{
//    if (!m_trackList.isEmpty () && m_currentIndex == -1)
//        return -1;
//    return m_currentIndex;
//}

//int PlayListMgr::count() const
//{
//    return m_trackList.size ();
//}

//AudioMetaList PlayListMgr::currentList() const
//{
//    return m_trackList;
//}

//AudioMetaObject PlayListMgr::currentTrack()
//{
//    if (m_trackList.isEmpty ())
//        return AudioMetaObject();
//    if (m_currentIndex >= m_trackList.size () || m_currentIndex < 0) {
//        return AudioMetaObject();
//    }
//    return m_trackList.value (m_currentIndex);
//}

//QObject *PlayListMgr::currentTrackObject() const
//{
//    return qobject_cast<QObject*>(currentTrack ());
//}

//AudioMetaObject PlayListMgr::get(int index) const
//{
//    if (m_trackList.isEmpty () || index >= m_trackList.size () || index < 0)
//        return AudioMetaObject();
//    return m_trackList.value (index);
//}

//void PlayListMgr::setRandom(bool random)
//{
//    m_random = random;
//}

//int PlayListMgr::randomIndex() const
//{
//    if (m_trackList.isEmpty ())
//        return -1;
//    QTime time = QTime::currentTime ();
//    qsrand(time.second () * 1000 + time.msec ());
//    int n = qrand ();
//    return n % m_trackList.size ();
//}

QStringList PlayListMgr::existPlayLists() const
{
    return m_existPlayLists;
}

bool PlayListMgr::open(const QString &name)
{
//    if (!m_trackList.isEmpty ()) {
////        qDeleteAll(m_trackList);
//        m_trackList.clear ();
//    }
//    m_currentIndex = -1;
    if (!isEmpty ())
        clear ();

    QString f = QString("%1/%2.%3").arg (m_playListDir).arg (name).arg (m_listFormat->extension ());
    qDebug()<<Q_FUNC_INFO<<"open playlist "<<f;
    if (!QFile::exists (f)) {
        qWarning()<<Q_FUNC_INFO<<"playlist not exists";
        return false;
    }
    QFile file(f);
    if (!file.open (QIODevice::ReadOnly)) {
        qWarning()<<Q_FUNC_INFO<<"open error: [%1]"<<file.errorString ();
        return false;
    }
    QTextStream stream(&file);
    QStringList fileList = m_listFormat->fileList (stream.readAll ());
    foreach (QString s, fileList) {
        QUrl url(s);
        QString str;
        if (url.isLocalFile ()) {
            str = url.toLocalFile ();
            if (!QFile::exists (str)) {
                qDebug()<<Q_FUNC_INFO<<QString("file [%1] not exist").arg (str);
                continue;
            }
            QFileInfo info(str);
//            meta = new AudioMetaObject(info.absolutePath (), info.fileName (), info.size (),this);
            AudioMetaObject o(info.absolutePath (), info.fileName (), info.size ());
            o.setMediaType ((int)Common::MediaTypeLocalFile);
//            meta->setMediaType ((int)Common::MediaTypeLocalFile);
            //TODO: should fill other SongMetaData properties from music library
//            m_trackList.append (o);
            addTrack (o);
        } else {
//            str = url.toString ();
//            meta = new AudioMetaObject(str, "", 0, this);
//            meta->setMediaType ((int)Common::MediaTypeUrl);
            AudioMetaObject o(url);
            o.setMediaType ((int)Common::MediaTypeUrl);
//            m_trackList.append (o);
            addTrack (o);
        }
//        m_trackList.append (meta);
    }
    file.close ();
    qDebug()<<Q_FUNC_INFO<<"get track list size "<<count ();
    return true;
}

bool PlayListMgr::save(const QString &fileName)
{
    if (isEmpty ()) {
        qDebug()<<Q_FUNC_INFO<<"track list is empty!!";
        return false;
    }
    QString f = QString("%1/%2.%3").arg (m_playListDir).arg (fileName).arg (m_listFormat->extension ());
    qDebug()<<Q_FUNC_INFO<<"try to save playlist "<<f;
    if (QFile::exists (f)) {
        qWarning()<<Q_FUNC_INFO<<"playlist already exists";
        return false;
    }
    QFile file(f);
    if (!file.open (QIODevice::WriteOnly)) {
        qWarning()<<Q_FUNC_INFO<<"open error: [%1]"<<file.errorString ();
        return false;
    }
    QTextStream s(&file);
    s << m_listFormat->format (currentList ());
    file.close ();
    return true;
}

void PlayListMgr::queryPlayLists()
{
    if (!m_playListDir.isEmpty ())
        m_playListDir.clear ();

    QDir dir(m_playListDir);
    if (!dir.exists ()) {
        qWarning()<<Q_FUNC_INFO<<"No playList dir exist!!";
        return;
    }
    dir.setFilter (QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    QFileInfoList list = dir.entryInfoList ();
    foreach (QFileInfo info, list) {
        QString s = info.completeSuffix ().toLower ();
        if (s.contains (m_listFormat->extension ().toLower ())
                || m_listFormat->extension ().toLower ().contains (s)) {
            m_existPlayLists.append (info.baseName ());
        }
    }
}

//int PlayListMgr::sizeLimit() const
//{
//    return -1;
//}

//void PlayListMgr::setCurrentIndex(int index)
//{
//    qDebug()<<Q_FUNC_INFO<<QString("set index to [%1], current indext [%2], list size [%3]")
//              .arg (index).arg (m_currentIndex).arg (m_trackList.size ());

//    if (index >= m_trackList.size () || index < 0)
//        return;
//    if (m_currentIndex != index) {
//        m_currentIndex = index;
//        emit currentIndexChanged(index);
//    }
//}


} //PhoenixPlayer
