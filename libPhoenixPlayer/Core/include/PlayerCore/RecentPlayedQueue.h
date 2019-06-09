//#ifndef RECENTPLAYEDQUEUE_H
//#define RECENTPLAYEDQUEUE_H

//#include <QObject>
//#include <QList>
//#include <QStringList>

//#include "libphoenixplayer_global.h"
//#include "PlayerCore/MusicQueue.h"

//namespace PhoenixPlayer {

//namespace MusicLibrary {
//    class MusicLibraryManager;
//}

//class RecentPlayedQueue : public MusicQueue
//{
//    Q_OBJECT
//public:
//    explicit RecentPlayedQueue(MusicLibrary::MusicLibraryManager *mgr, QObject *parent = Q_NULLPTR);
//    virtual ~RecentPlayedQueue();

//    void addAndFocus(const AudioMetaObject &song);
//    void addTrack(const AudioMetaObject &song);
//    void addTrack(const AudioMetaList &list);
//    void removeTrack(const int &index);
//    void removeTracks(const int &startPos, const int &endPos);

//public slots:
//    void setCurrentIndex(int index);

//private:
//    void queryList();
////    void save();
//private:
//    PhoenixPlayer::MusicLibrary::MusicLibraryManager    *m_libraryMgr;

//};

//} //PhoenixPlayer
//#endif // RECENTPLAYEDQUEUE_H
