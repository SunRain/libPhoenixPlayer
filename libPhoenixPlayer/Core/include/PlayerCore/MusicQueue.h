#ifndef MUSICQUEUE_H
#define MUSICQUEUE_H

#include <QObject>
#include <QList>
#include <QStringList>

#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {
class AudioMetaObject;

namespace MusicLibrary {
class IMusicLibraryDAO;
}

class LIBPHOENIXPLAYER_EXPORT MusicQueue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int count READ count CONSTANT)
public:
    explicit MusicQueue(QObject *parent = 0);
    virtual ~MusicQueue();

    void addTrack(const AudioMetaObject &song);
    ///
    /// \brief addAndFocus add track to list and set index to current track
    /// \param song
    ///
    void addAndFocus(const AudioMetaObject &song);
    void addTrack(const AudioMetaList &list);
    void removeTrack(const int &index);
    void removeTracks(const int &startPos, const int &endPos);
    void clear();
    bool isEmpty();

    AudioMetaList currentList() const;

    ///
    /// \brief currentTrack
    /// \return empty AudioMetaObject if no track exists
    ///
    AudioMetaObject currentTrack() const;
//    QObject *currentTrackObject() const;
    ///
    /// \brief get
    /// \param index
    /// \return empty AudioMetaObject if no track exists
    ///
    AudioMetaObject get(int index) const;

    ///
    /// \brief currentIndex
    /// \return -1 if current queue is empty
    ///
    int currentIndex() const;

    ///
    /// \brief count
    /// \return queue size
    ///
    int count() const;

protected:
    ///
    /// \brief sizeLimit limited size of the queue
    /// \param size <=0 if no limited
    ///
    void sizeLimit(int size = 20);

    ///
    /// \brief skipDuplicates default true
    /// \param skip
    ///
    void skipDuplicates(bool skip = true);

public slots:
    void setCurrentIndex(int index);

signals:
    void currentIndexChanged(int index);
    void queueChanged();

private:
    int m_currentIndex;
    int m_limitSize;
    bool m_skipDuplicates;
    AudioMetaList m_trackList;
};


} //PhoenixPlayer
#endif // MUSICQUEUE_H
