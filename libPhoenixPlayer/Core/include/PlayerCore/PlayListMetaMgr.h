#ifndef PLAYLISTMETAMGR_H
#define PLAYLISTMETAMGR_H

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


class LIBPHOENIXPLAYER_EXPORT PlayListMetaMgr : public QObject
{
    Q_OBJECT
protected:
    explicit PlayListMetaMgr(QObject *parent = Q_NULLPTR);
    static PlayListMetaMgr *createInstance();

public:
    enum UpdateMetaRet {
        OK = 0x0,
        NameConflict
    };
    static PlayListMetaMgr *instance();
    virtual ~PlayListMetaMgr();

    QList<PlayListMeta> metaList() const;

    ///
    /// \brief addMeta
    /// \param meta
    /// \return    True if list not contains this meta(same meta name and suffix)
    /// False if list contains this meta(same meta name and suffix)
    ///
    bool addMeta(const PlayListMeta &meta);

    ///
    /// \brief tryAdd try to add meta to list
    /// If list not contains this meta, signal addedMeta will be emmited
    /// If list contains this meta(same meta name and suffix), nothing happed
    /// \param meta
    ///
    void tryAdd(const PlayListMeta &meta);

    ///
    /// \brief deleteMeta
    /// \param meta
    ///
    void deleteMeta(const PlayListMeta &meta);

    void updateMeta(const PlayListMeta &old,  const PlayListMeta &newMeta);

    ///
    /// \brief create
    /// \return
    ///
    PlayListMeta create();

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
    void metaDataChanged(UpdateMetaRet ret, const PlayListMeta &old, const PlayListMeta &newMeta);

    void addedMeta(const PlayListMeta &meta);

    void deletedMeta(const PlayListMeta &meta);

private:
    void readDatabase();

private:
//    PPSettings   *m_settings;
//    QString     m_playListDir;
//    QStringList m_existPlayLists;
    int         m_nameSuffix;
    QString     m_dbPath;
//    QMap<QString, PlayListMeta> m_metaMap;
    QList<PlayListMeta> m_metaList;
};


} //PhoenixPlayer

#endif // PLAYLISTMETAMGR_H
