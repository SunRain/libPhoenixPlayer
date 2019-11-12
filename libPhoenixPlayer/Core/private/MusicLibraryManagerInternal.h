#ifndef MUSICLIBRARYMANAGERINTERNAL_H
#define MUSICLIBRARYMANAGERINTERNAL_H

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QMap>

#include "libphoenixplayer_global.h"
#include "AudioMetaObject.h"
#include "AudioMetaGroupObject.h"
#include "PluginMgr.h"

namespace PhoenixPlayer {

    class PPSettingsInternal;
    class PluginMgrInternal;

    namespace MusicLibrary {
        class IMusicLibraryDAO;
        class LocalMusicScanner;
    }

class LIBPHOENIXPLAYER_EXPORT MusicLibraryManagerInternal : public QObject
{
    Q_OBJECT
public:
    explicit MusicLibraryManagerInternal(QSharedPointer<PPSettingsInternal> set,
                                         QSharedPointer<PluginMgrInternal> loader,
                                         QObject *parent = Q_NULLPTR);

    virtual ~MusicLibraryManagerInternal();

    inline bool daoValid() const
    {
        return m_dao != Q_NULLPTR;
    }

    inline MusicLibrary::IMusicLibraryDAO *dao() const
    {
        return m_dao;
    }

    inline AudioMetaList tracks() const {
        return m_trackList;
    }

    inline AudioMetaList *trackList()
    {
        return &m_trackList;
    }

    inline QMap<QString, bool> *likeMap()
    {
        return &m_likeMap;
    }

    inline QMap<QString, int> *playCntMap()
    {
        return &m_playCntMap;
    }

    inline QMap<QString, LastPlayedMeta> *lastPlayedMap()
    {
        return &m_lastPlayedMap;
    }

    inline QMap<QString, AudioMetaGroupObject> *albumGroupMap()
    {
        return &m_albumGroupMap;
    }

    inline QMap<QString, AudioMetaGroupObject> *artistGroupMap()
    {
        return &m_artistGroupMap;
    }

    inline QMap<QString, AudioMetaGroupObject> *genreGroupMap()
    {
        return &m_genreGroupMap;
    }

    void addLastPlayedTime(const QString &hash);

    void addLastPlayedTime(const AudioMetaObject &obj);

    void addPlayedCount(const QString &hash);

    void addPlayedCount(const AudioMetaObject &obj);

    int playedCount(const QString &hash) const;

    int playedCount(const AudioMetaObject &obj) const;

    void setPlayedCount(const QString &hash, int count);

    void setPlayedCount(const AudioMetaObject &obj, int count);

signals:
    void libraryListSizeChanged();

private:
    void initDAO();

    void initList();

    void insertToAlbumGroupMap(const AudioMetaObject &obj);
    void insertToArtistGroupMap(const AudioMetaObject &obj);
    void insertToGenreGroupMap(const AudioMetaObject &obj);

private:
    QSharedPointer<PPSettingsInternal>  m_settings;
    QSharedPointer<PluginMgrInternal>   m_pluginMgr;

    PluginMetaData                      m_usedPluginMeta;

    MusicLibrary::IMusicLibraryDAO      *m_dao = Q_NULLPTR;
    AudioMetaList                       m_trackList;
    QMap<QString, bool>                 m_likeMap;
    QMap<QString, int>                  m_playCntMap;
    QMap<QString, LastPlayedMeta>       m_lastPlayedMap;
    QMap<QString, AudioMetaGroupObject> m_albumGroupMap;
    QMap<QString, AudioMetaGroupObject> m_artistGroupMap;
    QMap<QString, AudioMetaGroupObject> m_genreGroupMap;
};


} //PhoenixPlayer
#endif // MUSICLIBRARYMANAGERINTERNAL_H
