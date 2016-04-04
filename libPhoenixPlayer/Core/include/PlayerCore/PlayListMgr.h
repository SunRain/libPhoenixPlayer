#ifndef PLAYLISTMGR_H
#define PLAYLISTMGR_H

#include <QObject>
#include <QList>
#include <QStringList>

namespace PhoenixPlayer {
class AudioMetaObject;
class PluginLoader;
class PluginHost;
class Settings;

namespace MusicLibrary {
class IMusicLibraryDAO;
}

class PlayListFormat;
class PlayListMgr : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QObject* currentTrack READ currentTrackObject CONSTANT)
    Q_PROPERTY(int count READ count CONSTANT)
//    Q_PROPERTY(int randomIndex READ randomIndex CONSTANT)
    Q_PROPERTY(QStringList existPlayLists READ existPlayLists CONSTANT)

public:
    explicit PlayListMgr(QObject *parent = 0);
    virtual ~PlayListMgr();
    bool addTrack(const AudioMetaObject *song);
    bool addTrack(const QList<AudioMetaObject *> &list);
    bool removeTrack(int index);
    bool removeTracks(int startPos, int endPos);
    void clear();
    bool isEmpty();
    ///
    /// \brief currentIndex
    /// \return -1 if current playlist is empty
    ///
    int currentIndex() const;
    ///
    /// \brief count
    /// \return playlist size
    ///
    int count() const;

    ///
    /// \brief currentTrack
    /// \return nullptr if no track exists
    ///
    AudioMetaObject *currentTrack() const;
    QObject *currentTrackObject() const;
    ///
    /// \brief get
    /// \param index
    /// \return nullptr if no track exists
    ///
    AudioMetaObject *get(int index) const;

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
    virtual QStringList existPlayLists() const;
    ///
    /// \brief open 打开播放列表
    /// \param name 不包括扩展名和路径的播放列表名称
    /// \return
    ///
    Q_INVOKABLE virtual bool open(const QString &name);
    ///
    /// \brief save 保存播放列表
    /// \param playListName
    /// \return
    ///
    Q_INVOKABLE virtual bool save(const QString &fileName);

signals:
    void currentIndexChanged(int index);

protected:
    virtual void queryPlayLists();

public slots:
    void setCurrentIndex(int index);

private:
    int m_currentIndex;
//    bool m_random;

    QString m_playListDir;
    QStringList m_existPlayLists;
    QList<AudioMetaObject *> m_trackList;

    Settings *m_settings;
    PlayListFormat *m_listFormat;
};
} //PhoenixPlayer
#endif // PLAYLISTMGR_H
