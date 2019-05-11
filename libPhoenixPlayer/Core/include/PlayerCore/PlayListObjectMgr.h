#ifndef PLAYLISTOBJECTMGR_H
#define PLAYLISTOBJECTMGR_H

#include <QObject>
#include <QList>
#include <QStringList>

#include "libphoenixplayer_global.h"
#include "PlayerCore/PlayListObject.h"
#include "PlayerCore/PlayListMeta.h"

namespace PhoenixPlayer {
class AudioMetaObject;
class PluginLoader;
class PluginHost;
class PPSettings;

namespace MusicLibrary {
class IMusicLibraryDAO;
}


class LIBPHOENIXPLAYER_EXPORT PlayListObjectMgr : public QObject
{
    Q_OBJECT
protected:
    explicit PlayListObjectMgr(QObject *parent = Q_NULLPTR);
    static PlayListObjectMgr *createInstance();

public:
    static PlayListObjectMgr *instance();
    virtual ~PlayListObjectMgr();

    QList<PlayListMeta> metaList() const;

    bool addMeta(const PlayListMeta &meta);

    ///
    /// \brief tryAdd try to add meta to list
    /// If list not contains this meta, signal addedMeta will be emmited
    /// If list contains this meta(same meta name and suffix), nothing happed
    /// \param meta
    ///
    void tryAdd(const PlayListMeta &meta);

    void deleteMeta(const PlayListMeta &meta);

    void updateMeta(const PlayListMeta &old,  const PlayListMeta &newMeta);

//    void refresh();

//    Q_INVOKABLE void refreshExistPlayLists();

//    ///
//    /// \brief existPlayLists
//    /// \return  返回当前目录下支持的播放列表文件，不包括播放列表的扩展名和路径
//    /// /path/to/xxx.m3u ==> xxx
//    ///
//    QStringList existPlayLists() const;

//    void setExistPlayLists(const QStringList &existPlayLists);

    void saveToDatabase();

protected:
//    virtual void queryPlayLists();

signals:
//    void existPlayListsChanged(const QStringList &existPlayLists);
    void metaDataChanged(const PlayListMeta &old, const PlayListMeta &newMeta);

    void addedMeta(const PlayListMeta &meta);

    void deletedMeta(const PlayListMeta &meta);

private:
    void readDatabase();

private:
//    PPSettings   *m_settings;
//    QString     m_playListDir;
//    QStringList m_existPlayLists;
    QString     m_dbPath;
    QMap<QString, PlayListMeta> m_metaMap;
};


} //PhoenixPlayer

#endif // PLAYLISTOBJECTMGR_H
