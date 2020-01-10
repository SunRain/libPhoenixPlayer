#ifndef IMUSICLIBRARYDAO_H
#define IMUSICLIBRARYDAO_H

#include <QObject>
#include <QVariant>

#include "PPCommon.h"
#include "AudioMetaObject.h"
#include "BasePlugin.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

class IMusicLibraryDAO : public BasePlugin
{
    Q_OBJECT
public:
    explicit IMusicLibraryDAO(QObject *parent = Q_NULLPTR);
    virtual ~IMusicLibraryDAO() override;

    virtual bool initDataBase() = 0;

    ///
    /// \brief insertMetaData insert track to database
    /// \param metaData the track
    /// \param skipDuplicates skip if track already exists in database
    /// \return
    ///
    bool insertMetaData(const AudioMetaObject &obj, bool skipDuplicates = true);
    ///
    /// \brief updateMetaData update track meta
    /// \param metaData the track
    /// \param skipEmptyValue use origin value if new track has some empty values
    /// \return
    ///
    bool updateMetaData(const AudioMetaObject &obj, bool skipEmptyValue = true);
    bool deleteMetaData(const AudioMetaObject &obj);
    bool deleteByHash(const QString &hash);

    virtual AudioMetaObject trackFromHash(const QString &hash) const = 0;

    virtual QStringList trackHashList() const = 0;

    virtual QStringList trackHashListByPlayedCount(bool orderByDesc = true) const = 0;

    virtual QStringList trackHashListByLastPlayedTime(bool orderByDesc = true) const = 0;

    virtual bool setLike(const QString &hash, bool like) = 0;

    virtual bool isLike(const QString &hash) const = 0;

    ///
    /// \brief setPlayedCount set count of a track played
    /// \param hash
    /// \param count
    /// \return
    ///
    virtual bool setPlayedCount(const QString &hash, int count) = 0;

    virtual int playedCount(const QString &hash) const = 0;

    virtual bool setLastPlayedTime(const QString &hash, qint64 secs) = 0;

    virtual bool setLastPlayedTime(const LastPlayedMeta &meta) = 0;

    virtual qint64 getLastPlayedTime(const QString &hash) const = 0;

    virtual LastPlayedMeta getLastPlayedMeta(const QString &hash) const = 0;

    ///
    /// \brief getLastPlayedMeta
    /// \param limit limit size
    /// \param orderByDesc
    /// TRUE order by timestamp with desc
    /// FALSE order by timestamp with asc
    /// \return
    ///
    virtual QList<LastPlayedMeta> getLastPlayedMeta(int limit = 20, bool orderByDesc = true) const = 0;

    ///
    /// \brief getLastPlayedByAlbum
    /// \param limit the limit of album size
    /// \param orderByDesc
    /// TRUE order by timestamp with desc
    /// FALSE order by timestamp with asc
    /// \return
    ///
    virtual QList<LastPlayedMeta> getLastPlayedByAlbum(int limit = 20, bool orderByDesc = true) const = 0;

    virtual QList<LastPlayedMeta> getLastPlayedByArtist(int limit = 20, bool orderByDesc = true) const = 0;

    virtual QList<LastPlayedMeta> getLastPlayedByGenres(int limit = 20, bool orderByDesc = true) const = 0;

    virtual void insertSpectrumData(const AudioMetaObject &obj, const QList<QList<qreal>> &list) = 0;

    virtual QList<QList<qreal>> getSpectrumData(const AudioMetaObject &obj) const = 0;

    // QObject interface
public:
    bool event(QEvent *event) Q_DECL_OVERRIDE;

    // BasePlugin interface
public:
    virtual PluginProperty property() const Q_DECL_OVERRIDE
    {
        return PluginProperty();
    }

    virtual PluginType type() const Q_DECL_OVERRIDE
    {
        return PluginMusicLibraryDAO;
    }

protected:
    virtual bool doInsertMetaData(const AudioMetaObject &obj, bool skipDuplicates = true) = 0;
    virtual bool doUpdateMetaData(const AudioMetaObject &obj, bool skipEmptyValue = true) = 0;
    virtual bool doDeleteByHash(const QString &hash) = 0;

signals:
    void metaDataInserted(const QString &hash);
    void metaDataDeleted(const QString &hash);
    void metaDataChanged(const QString &hash);

public slots:
    ///
    /// \brief beginTransaction 开始事务
    /// \return
    ///
    virtual bool beginTransaction() = 0;

    ///
    /// \brief commitTransaction 提交/结束事务
    /// \return
    ///
    virtual bool commitTransaction() = 0;
};

} //IMUSICLIBRARYDAO_H
} //PhoenixPlayer

#endif // IPLAYLISTDAO_H
