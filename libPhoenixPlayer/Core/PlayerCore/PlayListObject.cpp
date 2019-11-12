#include "PlayerCore/PlayListObject.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>

#include <QtMultimedia/QMediaPlaylist>

#include "PluginLoader.h"
#include "MusicLibrary/IMusicLibraryDAO.h"

#include "PPSettings.h"
#include "AudioMetaObject.h"

#include "PlayerCore/PlayListFormat.h"
#include "PlayerCore/M3uPlayListFormat.h"

namespace PhoenixPlayer {

PlayListObject::PlayListObject(const PlayListMeta &meta, QObject *parent)
    : MusicQueue(parent)
    , m_meta(meta)
{
    setSizeLimit (-1);
    setSkipDuplicates (true);

    //TODO support different playlist format and use global(singtone) format function
    m_listFormat = new M3uPlayListFormat(this);
}

PlayListObject::~PlayListObject()
{
    save();
}

//void PlayListObject::refreshExistPlayLists()
//{
//    queryPlayLists();
//}

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

//QStringList PlayListObject::existPlayLists() const
//{
//    return m_existPlayLists;
//}

bool PlayListObject::open()
{

    if (!isEmpty ())
        clear ();

    //TODO choose best listformat if more supported listformat exist
    //TODO m_listFormat->extension() is same as meta->getFileSuffix() currently
    QString f = QString("%1/%2.%3").arg(m_meta.getDir()).arg(m_meta.getFileName()).arg(m_listFormat->extension());
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
        if (url.isLocalFile ()
                || !s.toLower().startsWith("http://")
                || !s.toLower().startsWith("ftp://")) { // local files
            str = url.toLocalFile (); //if scheme is "file"
            //TODO url.toLocalFile will return schemes, like "file://path/to/file" ??
            if (str.isEmpty()) { //no "file://" scheme
                str = s;
            }
            if (!QFile::exists (str)) {
                qDebug()<<Q_FUNC_INFO<<QString("file [%1] not exist").arg (str);
                continue;
            }
            QFileInfo info(str);
            QString path = info.absolutePath();
            QString name = info.fileName();
            quint64 size = info.size();

            AudioMetaObject o(info.absolutePath (), info.fileName (), info.size ());
//            qDebug()<<Q_FUNC_INFO<<QString("===== Add file name=[%1],path=[%2],size=[%3],hash=[%4]")
//                      .arg(name).arg(path).arg(QString::number(size)).arg(o.hash());
            o.setMediaType ((int)PPCommon::MediaTypeLocalFile);
            //TODO: should fill other SongMetaData properties from music library
            addTrack (o);
        } else { //network stream
            AudioMetaObject o(url);
            o.setMediaType ((int)PPCommon::MediaTypeUrl);
            addTrack (o);
        }
    }
    file.close ();
    qDebug()<<Q_FUNC_INFO<<"get track list size "<<count ();
    return true;
}

bool PlayListObject::save()
{
    if (isEmpty ()) {
        qWarning()<<Q_FUNC_INFO<<"track list is empty!!";
    }
    //TODO choose best listformat if more supported listformat exist
    //TODO m_listFormat->extension() is same as meta->getFileSuffix() currently
    QString f = QString("%1/%2.%3").arg (m_meta.getDir()).arg(m_meta.getFileName()).arg(m_listFormat->extension ());
    qDebug()<<Q_FUNC_INFO<<"try to save playlist "<<f;
    if (QFile::exists (f)) {
        if (!QFile::remove(f)) {
            qWarning()<<Q_FUNC_INFO<<"override playlist fail";
            return false;
        }
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

PlayListMeta PlayListObject::meta() const
{
    return m_meta;
}


//bool PlayListObject::create(const QString &name)
//{
//    if (name.isEmpty()) {
//        return false;
//    }
//    m_fileName = name;
//    return this->save();
//}

//bool PlayListObject::create(const QString &name, const AudioMetaList &list)
//{
//    if (name.isEmpty()) {
//        return false;
//    }
//    m_fileName = name;
//    addTrack(list);
//    return this->save();
//}

//void PlayListObject::queryPlayLists()
//{
//    QDir dir(m_playListDir);
//    if (!dir.exists ()) {
//        qWarning()<<Q_FUNC_INFO<<"No playList dir exist!!";
//        return;
//    }
//    dir.setFilter (QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
//    QFileInfoList list = dir.entryInfoList ();
//    QStringList plst;
//    foreach (QFileInfo info, list) {
//        QString s = info.completeSuffix ().toLower ();
//        if (s.contains (m_listFormat->extension ().toLower ())
//                || m_listFormat->extension ().toLower ().contains (s)) {
//            plst.append (info.baseName ());
//        }
//    }
//    setExistPlayLists(plst);
//}

//void PlayListObject::setExistPlayLists(QStringList existPlayLists)
//{
//    if (m_existPlayLists == existPlayLists)
//        return;

//    m_existPlayLists = existPlayLists;
//    emit existPlayListsChanged(existPlayLists);
//}

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
