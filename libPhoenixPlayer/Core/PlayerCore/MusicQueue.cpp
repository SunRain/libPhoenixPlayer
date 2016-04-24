#include "PlayerCore/MusicQueue.h"

#include <QDebug>

#include "AudioMetaObject.h"

namespace PhoenixPlayer {

MusicQueue::MusicQueue(QObject *parent)
    : QObject(parent)
    , m_currentIndex(-1)
    , m_limitSize(20)
    , m_skipDuplicates(true)
{

}

MusicQueue::~MusicQueue()
{
    if (!m_trackList.isEmpty ()) {
        m_trackList.clear ();
    }
}

void MusicQueue::addTrack(const AudioMetaObject &song)
{
    if (song.isEmpty ())
        return;
    if (m_skipDuplicates && m_trackList.contains (song))
        return;
    if (m_limitSize >0 && m_trackList.size () >= m_limitSize)
        m_trackList.removeFirst ();
    m_trackList.append (song);
    emit queueChanged ();
}

void MusicQueue::addTrack(const AudioMetaList &list)
{
    if (list.isEmpty ())
        return;
    AudioMetaList l;
    if (m_skipDuplicates) {
        foreach (AudioMetaObject o, list) {
            if (m_trackList.contains (o))
                continue;
            l.append (o);
        }
    } else {
        l = list;
    }
    if (m_limitSize >0 && m_trackList.size () + l.size ()>= m_limitSize) {
        m_trackList = m_trackList.mid (m_trackList.size ()+l.size ()- m_limitSize);
    }
    m_trackList.append (l);
    emit queueChanged ();
}

void MusicQueue::removeTrack(const int &index)
{
    if (index < 0 || m_trackList.isEmpty () || index >= m_trackList.size ()) {
        qDebug()<<Q_FUNC_INFO<<"invalid index";
        return;
    }
    m_trackList.removeAt (index);
    emit queueChanged ();
}

void MusicQueue::removeTracks(const int &startPos, const int &endPos)
{
    if (startPos < 0 || endPos < 0 || startPos > endPos
            || m_trackList.isEmpty ()
            || startPos >= m_trackList.size () || endPos >= m_trackList.size ()) {
        qDebug()<<Q_FUNC_INFO<<"invalid pos";
        return;
    }
    AudioMetaList tmp;
    for (int i=0; i<m_trackList.size (); ++i) {
        if (i >= startPos && i <= endPos) {
            continue;
        }
        tmp.append (m_trackList.value (i));
    }
//    m_trackList.clear ();
//    m_trackList.append (tmp);
    m_trackList.swap (tmp);
    emit queueChanged ();
}

void MusicQueue::clear()
{
    m_currentIndex = -1;
    if (!m_trackList.isEmpty ()) {
        m_trackList.clear ();
        emit queueChanged ();
    }
}

bool MusicQueue::isEmpty()
{
    return m_trackList.isEmpty ();
}

AudioMetaList MusicQueue::currentList() const
{
    return m_trackList;
}

AudioMetaObject MusicQueue::currentTrack() const
{
//    if (m_trackList.isEmpty ())
//        return AudioMetaObject();
//    if (m_currentIndex >= m_trackList.size () || m_currentIndex < 0) {
//        return AudioMetaObject();
//    }
    return m_trackList.value (m_currentIndex);
}

AudioMetaObject MusicQueue::get(int index) const
{
//    if (m_trackList.isEmpty () || index >= m_trackList.size () || index < 0)
//        return AudioMetaObject();
    return m_trackList.value (index);
}

int MusicQueue::currentIndex() const
{
    if (!m_trackList.isEmpty () && m_currentIndex == -1)
        return -1;
    return m_currentIndex;
}

int MusicQueue::count() const
{
    return m_trackList.size ();
}

void MusicQueue::sizeLimit(int size)
{
    m_limitSize = size;
}

void MusicQueue::skipDuplicates(bool skip)
{
    m_skipDuplicates = skip;
}

void MusicQueue::setCurrentIndex(int index)
{
    qDebug()<<Q_FUNC_INFO<<QString("set index to [%1], current indext [%2], list size [%3]")
              .arg (index).arg (m_currentIndex).arg (m_trackList.size ());

    if (index >= m_trackList.size () || index < 0)
        return;
    if (m_currentIndex == index)
        return;
    m_currentIndex = index;
    emit currentIndexChanged(index);
}

}
