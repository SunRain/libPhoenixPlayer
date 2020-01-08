#ifndef MUSICLIBRARYMANAGER_H
#define MUSICLIBRARYMANAGER_H

#include <QObject>
#include <QPointer>
#include <QStringList>
#include <QList>
#include <QSharedPointer>

#include "libphoenixplayer_global.h"
#include "PPCommon.h"
#include "BaseObject.h"
#include "AudioMetaObject.h"
#include "AudioMetaGroupObject.h"

namespace PhoenixPlayer {
    class PPSettings;
//    class LibPhoenixPlayer;
    class MusicLibraryManagerInternal;


    namespace MusicLibrary {
        class IMusicLibraryDAO;
        class LocalMusicScanner;
/*!
 * \brief The MusicLibraryManager class
 * Internal Singleton class
 */
class LIBPHOENIXPLAYER_EXPORT MusicLibraryManager : public BaseObject
{
    Q_OBJECT
public:
    explicit MusicLibraryManager(QObject *parent = Q_NULLPTR);
    virtual ~MusicLibraryManager();

    /*!
     * \brief allTracks 显示所有曲目
     * \return 空列表如果没有曲目
     */
    AudioMetaList allTracks() const;

    AudioMetaObject trackFromHash(const QString &hash) const;

    /*!
     * \brief empty wether current library is empty
     * \return true if empty
     */
    bool empty() const;

    inline bool isEmpty() const {return empty();}

    void deleteObject(const AudioMetaObject &obj, bool deleteFromLocalDisk = false);

    void deleteObject(const QString &hash, bool deleteFromLocalDisk = false);

    /*!
     * \brief artistTracks 显示某个artist下的曲目
     * \param artistName artist名
     * \param limitNum 显示个数. <=0, no limit
     * \return 空列表如果没有曲目
     */
    AudioMetaList artistTracks(const QString &artistName,int limitNum = 0);
    AudioMetaList albumTracks(const QString  &albumName, int limitNum = 0);
    AudioMetaList genreTracks(const QString &genreName, int limitNum = 0);
    AudioMetaList mediaTypeTracks(const QString &mediaType, int limitNum = 0);
    AudioMetaList userRatingTracks(const QString &rating, int limitNum = 0);
    AudioMetaList folderTracks(const QString &folder, int limitNum = 0);

    AudioMetaGroupList artistList() const;
    AudioMetaGroupList albumList() const;
    AudioMetaGroupList genreList() const;

    void setLike(const QString &hash, bool like);
    void setLike(const AudioMetaObject &obj, bool like);

    bool isLike(const QString &hash);
    bool isLike(const AudioMetaObject &obj);

    /*!
     * \brief addLastPlayedTime add last played time for current time
     * \param hash
     */
    void addLastPlayedTime(const QString &hash);

    void addLastPlayedTime(const AudioMetaObject &obj);

    qint64 getLastPlayedTime(const QString &hash);

    qint64 getLastPlayedTime(const AudioMetaObject &obj);

    /*!
     * \brief setPlayedCount set count of a track played
     * \param hash
     * \param count
     */
    void setPlayedCount(const QString &hash, int count);
    void setPlayedCount(const AudioMetaObject &obj, int count);
    void addPlayedCount(const QString &hash);
    void addPlayedCount(const AudioMetaObject &obj);

    int playedCount(const QString &hash) const;

    int playedCount(const AudioMetaObject &obj) const;

    LastPlayedMeta getLastPlayedMeta(const QString &hash) const;

    /*!
     * \brief getLastPlayedMeta
     * \param limit
     * \param orderByDesc
     * TRUE order by timestamp with desc
     * FALSE order by timestamp with asc
     * \return
     */
    QList<LastPlayedMeta> getLastPlayedMeta(int limit = 20, bool orderByDesc = true) const;

    /*!
     * \brief getLastPlayedByAlbum
     * \param limit the limit of album size
     * \param orderByDesc
     * TRUE order by timestamp with desc
     * FALSE order by timestamp with asc
     * \return
     */
    QList<LastPlayedMeta> getLastPlayedByAlbum(int limit = 20, bool orderByDesc = true) const;

    QList<LastPlayedMeta> getLastPlayedByArtist(int limit = 20, bool orderByDesc = true) const;

    QList<LastPlayedMeta> getLastPlayedByGenres(int limit = 20, bool orderByDesc = true) const;

    QStringList trackHashListByPlayedCount(bool orderByDesc = true) const;

    QStringList trackHashListByLastPlayedTime(bool orderByDesc = true) const;


    void saveToDB();

    /*!
     * \brief loadSpectrumData
     * \param obj
     * \return empty if no spectrum data file found
     */
    QList<QList<qreal> > loadSpectrumData(const AudioMetaObject &obj);

signals:
    /*!
     * \brief libraryListSizeChanged
     * emited when audio meta object removed from library or added to library
     */
    void libraryListSizeChanged();

private:
    QSharedPointer<MusicLibraryManagerInternal> m_internal;
};

} //MusicLibrary
} //PhoenixPlayer

#endif // MUSICLIBRARYMANAGER_H
