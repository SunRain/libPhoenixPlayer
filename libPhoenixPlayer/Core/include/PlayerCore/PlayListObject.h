#ifndef PLAYLISTOBJECT_H
#define PLAYLISTOBJECT_H

#include <QObject>
#include <QList>
#include <QStringList>

#include "libphoenixplayer_global.h"
#include "PlayerCore/MusicQueue.h"
#include "PlayerCore/PlayListMeta.h"

namespace PhoenixPlayer {
class AudioMetaObject;
class PluginLoader;
class PluginHost;
class PPSettings;

namespace MusicLibrary {
class IMusicLibraryDAO;
}

class PlayListFormat;
class LIBPHOENIXPLAYER_EXPORT PlayListObject : public MusicQueue
{
    Q_OBJECT
//    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
//    Q_PROPERTY(int count READ count CONSTANT)
//    Q_PROPERTY(QStringList existPlayLists READ existPlayLists WRITE setExistPlayLists NOTIFY existPlayListsChanged)

public:
    explicit PlayListObject(const PlayListMeta &meta, QObject *parent = Q_NULLPTR);
//    explicit PlayListObject(const QString &playlistDir, QObject *parent = Q_NULLPTR);
    virtual ~PlayListObject();

    ///
    /// \brief refreshExistPlayLists
    ///
//    Q_INVOKABLE void refreshExistPlayLists();
    ///
    /// \brief existPlayLists
    /// \return  返回当前目录下支持的播放列表文件，不包括播放列表的扩展名和路径
    /// /path/to/xxx.m3u ==> xxx
    ///
//    QStringList existPlayLists() const;

    ///
    /// \brief open 打开播放列表
    /// \return
    ///
    Q_INVOKABLE bool open();
    ///
    /// \brief save 保存播放列表
    /// \return
    ///
    Q_INVOKABLE bool save();

    PlayListMeta meta() const;

//    QString playListDir() const;
//    void setPlayListDir(const QString &playListDir);

    ///
    /// \brief create create new playlist object with given name
    /// \param name
    /// \return
    ///
//    bool create(const QString &name);

//    bool create(const QString &name, const AudioMetaList &list);

signals:
    //    void currentIndexChanged(int index);
    //    void trackAdded(const AudioMetaObject &object);
//    void trackListAdded(const AudioMetaList &list);
//    void trackRemoved(int index);
//    void trackListRemoved(const AudioMetaList &list);

//    void existPlayListsChanged(const QStringList &existPlayLists);

protected:
//    virtual void queryPlayLists();

public slots:
//    void setExistPlayLists(QStringList existPlayLists);

private:
//    int m_currentIndex;
////    bool m_random;

//    QString m_playListDir;
//    QString m_fileName;
//    QStringList m_existPlayLists;
//    AudioMetaList m_trackList;

//    Settings *m_settings;
    PlayListFormat *m_listFormat;

    PlayListMeta m_meta;

};
} //PhoenixPlayer
#endif // PLAYLISTOBJECT_H
