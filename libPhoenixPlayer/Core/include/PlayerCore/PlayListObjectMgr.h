#ifndef PLAYLISTOBJECTMGR_H
#define PLAYLISTOBJECTMGR_H

#include <QObject>
#include <QList>
#include <QStringList>

#include "libphoenixplayer_global.h"
#include "PlayerCore/PlayListObject.h"

namespace PhoenixPlayer {
class AudioMetaObject;
class PluginLoader;
class PluginHost;
class Settings;

namespace MusicLibrary {
class IMusicLibraryDAO;
}


class LIBPHOENIXPLAYER_EXPORT PlayListObjectMgr : public QObject
{
    Q_OBJECT
public:
    explicit PlayListObjectMgr(Settings *set, QObject *parent = Q_NULLPTR);
    virtual ~PlayListObjectMgr();

    Q_INVOKABLE void refreshExistPlayLists();

    ///
    /// \brief existPlayLists
    /// \return  返回当前目录下支持的播放列表文件，不包括播放列表的扩展名和路径
    /// /path/to/xxx.m3u ==> xxx
    ///
    QStringList existPlayLists() const;

    void setExistPlayLists(const QStringList &existPlayLists);

protected:
    virtual void queryPlayLists();

signals:
    void existPlayListsChanged(const QStringList &existPlayLists);

private:
    Settings   *m_settings;
    QString     m_playListDir;
    QStringList m_existPlayLists;
};


} //PhoenixPlayer

#endif // PLAYLISTOBJECTMGR_H
