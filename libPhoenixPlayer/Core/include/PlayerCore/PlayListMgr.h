#ifndef PLAYLISTMGR_H
#define PLAYLISTMGR_H

#include <QObject>
#include <QList>
#include <QStringList>

#include "libphoenixplayer_global.h"
#include "PlayerCore/MusicQueue.h"

namespace PhoenixPlayer {
class AudioMetaObject;
class PluginLoader;
class PluginHost;
class Settings;

namespace MusicLibrary {
class IMusicLibraryDAO;
}

class PlayListFormat;
class LIBPHOENIXPLAYER_EXPORT PlayListMgr : public MusicQueue
{
    Q_OBJECT
//    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
//    Q_PROPERTY(int count READ count CONSTANT)
    Q_PROPERTY(QStringList existPlayLists READ existPlayLists CONSTANT)

public:
    explicit PlayListMgr(Settings *set, QObject *parent = 0);
    virtual ~PlayListMgr();
//    void addTrack(const AudioMetaObject &song);
//    void addTrack(const AudioMetaList &list);
//    void removeTrack(const int &index);
//    void removeTracks(const int &startPos, const int &endPos);
//    void clear();
//    bool isEmpty();
    ///
    /// \brief currentIndex
    /// \return -1 if current playlist is empty
    ///
//    int currentIndex() const;
    ///
    /// \brief count
    /// \return playlist size
    ///
//    int count() const;

//    AudioMetaList currentList() const;

    ///
    /// \brief currentTrack
    /// \return nullptr if no track exists
    ///
//    AudioMetaObject currentTrack() const;
//    QObject *currentTrackObject() const;
    ///
    /// \brief get
    /// \param index
    /// \return nullptr if no track exists
    ///
//    AudioMetaObject get(int index) const;

//    void setRandom(bool random);
    ///
    /// \brief randomIndex
    /// \return -1 if no tracks in current playlist
    ///
//    int randomIndex() const;

    ///
    /// \brief existPlayLists
    /// \return  返回当前目录下支持的播放列表文件，不包括播放列表的扩展名和路径
    /// /path/to/xxx.m3u ==> xxx
    ///
    QStringList existPlayLists() const;
    ///
    /// \brief open 打开播放列表
    /// \param name 不包括扩展名和路径的播放列表名称
    /// \return
    ///
    Q_INVOKABLE bool open(const QString &name);
    ///
    /// \brief save 保存播放列表
    /// \param playListName
    /// \return
    ///
    Q_INVOKABLE bool save(const QString &fileName);

signals:
//    void currentIndexChanged(int index);
//    void trackAdded(const AudioMetaObject &object);
//    void trackListAdded(const AudioMetaList &list);
//    void trackRemoved(int index);
//    void trackListRemoved(const AudioMetaList &list);

protected:
    virtual void queryPlayLists();

public slots:
//    void setCurrentIndex(int index);

private:
//    int m_currentIndex;
////    bool m_random;

    QString m_playListDir;
    QStringList m_existPlayLists;
//    AudioMetaList m_trackList;

    Settings *m_settings;
    PlayListFormat *m_listFormat;

};
} //PhoenixPlayer
#endif // PLAYLISTMGR_H
