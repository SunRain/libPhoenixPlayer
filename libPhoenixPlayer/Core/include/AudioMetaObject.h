#ifndef AUDIOMETAOBJECT_H
#define AUDIOMETAOBJECT_H

#include <QSharedDataPointer>
#include <QVariant>
#include <QDate>
#include <QUrl>

#include "libphoenixplayer_global.h"
#include "PPCommon.h"
#include "../AudioMetaObject_p.h"

typedef QList<PhoenixPlayer::AudioMetaObject> AudioMetaList;

class QStringList;
class QDate;
class QJsonObject;
namespace PhoenixPlayer {

class LIBPHOENIXPLAYER_EXPORT LastPlayedMeta
{
public:
    LastPlayedMeta();
    LastPlayedMeta(const LastPlayedMeta &other);
    virtual ~LastPlayedMeta();

    inline LastPlayedMeta &operator =(const LastPlayedMeta &other) {
        if (this != &other)
            d.operator = (other.d);
        return *this;
    }

    bool operator == (const LastPlayedMeta &other);

    inline bool operator != (const LastPlayedMeta &other) {
        return !operator == (other);
    }

    QString audioMetaObjHash() const;
    QString albumName() const;
    QString artistName() const;
    QString genres() const;
    qint64 timestamp() const;

    void setAudioMetaObjHash(const QString &audioMetaObjHash);
    void setAlbumName(const QString &name);
    void setArtistName(const QString &name);
    void setGenres(const QString &genres);
    void setTimestamp(qint64 time);

    ///
    /// \brief isValid valid object hash is not empty && (timestamp != 0)
    /// \return
    ///
    bool isValid() const;

    ///
    /// \brief fromAudioMetaObject
    /// \param obj with timestamp QDateTime::currentSecsSinceEpoch()
    /// \return
    ///
    static LastPlayedMeta fromAudioMetaObject(const AudioMetaObject &obj);

    static qint64 placeholderTimestamp();

private:
    QSharedDataPointer<LastPlayedMetaPriv> d;
};


///
/// \brief The AlbumMeta class
///
class LIBPHOENIXPLAYER_EXPORT AlbumMeta
{
public:
    AlbumMeta();
    AlbumMeta(const AlbumMeta &other);
    virtual ~AlbumMeta();

    inline AlbumMeta &operator =(const AlbumMeta &other) {
        if (this != &other)
            d.operator = (other.d);
        return *this;
    }
    bool operator == (const AlbumMeta &other);
    inline bool operator != (const AlbumMeta &other) {
        return !operator == (other);
    }

    QString name() const;
    QUrl imgUri() const;
    QString description() const;
    QString date() const;

    void setName(const QString &name);
    void setImgUri(const QUrl &uri);
    void setDescription(const QString &description);
    void setDate(const QString &date);

    QJsonObject toObject() const;
    QByteArray toJson() const;
    QVariantMap toMap() const;
    static AlbumMeta fromMap(const QVariantMap &map);
    static AlbumMeta fromJson(const QByteArray &json);

private:
    QSharedDataPointer<AlbumMetaPriv> d;
};

///
/// \brief The ArtistMeta class
///
class LIBPHOENIXPLAYER_EXPORT ArtistMeta
{
public:
    ArtistMeta();
    ArtistMeta(const ArtistMeta &other);
    virtual ~ArtistMeta();
    inline ArtistMeta &operator =(const ArtistMeta &other) {
        if (this != &other)
            d.operator = (other.d);
        return *this;
    }
    bool operator == (const ArtistMeta &other);
    inline bool operator != (const ArtistMeta &other) {
        return !operator == (other);
    }

    QString name() const;
    QUrl imgUri() const;
    QString description() const;

    void setName(const QString &name);
    void setImgUri(const QUrl &uri);
    void setDescription(const QString &description);

    QJsonObject toObject() const;
    QByteArray toJson() const;
    QVariantMap toMap() const;
    static ArtistMeta fromMap(const QVariantMap &map);
    static ArtistMeta fromJson(const QByteArray &json);

private:
    QSharedDataPointer<ArtistMetaPriv> d;
};

///
/// \brief The CoverMeta class
///
class LIBPHOENIXPLAYER_EXPORT CoverMeta
{
public:
    CoverMeta();
    CoverMeta(const CoverMeta &other);
    virtual ~CoverMeta();
    inline CoverMeta &operator =(const CoverMeta &other) {
        if (this != &other)
            d.operator = (other.d);
        return *this;
    }
    bool operator == (const CoverMeta &other);
    inline bool operator != (const CoverMeta &other) {
        return !operator == (other);
    }

    QUrl smallUri() const;
    QUrl middleUri() const;
    QUrl largeUri() const;

    void setSmallUri(const QUrl &small);
    void setMiddleUri(const QUrl &middle);
    void setLargeUri(const QUrl &large);

    QJsonObject toObject() const;
    QByteArray toJson() const;
    QVariantMap toMap() const;
    static CoverMeta fromMap(const QVariantMap &map);
    static CoverMeta fromJson(const QByteArray &json);

private:
    QSharedDataPointer<CoverMetaPriv> d;
};

//    E_Category,
//    E_Keywords,
//    E_Language,
//    E_Publisher,
//    E_Copyright,
//    E_Mood,
//    E_Composer,
//    E_Conductor	,
//    E_Comment,
///
/// \brief The TrackMeta class
///
class LIBPHOENIXPLAYER_EXPORT TrackMeta
{
public:
    TrackMeta();
    TrackMeta(const TrackMeta &other);
    virtual ~TrackMeta();

    inline TrackMeta &operator =(const TrackMeta &other) {
        if (this != &other)
            d.operator = (other.d);
        return *this;
    }
    bool operator == (const TrackMeta &other);
    inline bool operator != (const TrackMeta &other) {
        return !operator == (other);
    }

    QString bitRate() const;
    int duration() const;
    QString title() const;
    QString description() const;
    QString year() const;
    QString date() const;
    QString genre() const;
    QString sampleRate() const;
    QString userRating() const;

    void setBitRate(const QString &arg);
    void setDuration(int arg);
    void setTitle(const QString &arg);
    void setDescription(const QString &arg);
    void setYear(const QString &arg);
    void setDate(const QString &arg);
    void setGenre(const QString &arg);
    void setSampleRate(const QString &arg);
    void setUserRating(const QString &arg);

    QJsonObject toObject() const;
    QByteArray toJson() const;
    QVariantMap toMap() const;
    static TrackMeta fromMap(const QVariantMap &map);
    static TrackMeta fromJson(const QByteArray &json);

private:
    QSharedDataPointer<TrackMetaPriv> d;
};

class AudioMetaObjectPriv;
///
/// \brief The AudioMetaObject class
///
class LIBPHOENIXPLAYER_EXPORT AudioMetaObject
{
public:
    AudioMetaObject();
    AudioMetaObject(const QString &path, const QString &name, quint64 size);
    AudioMetaObject(const AudioMetaObject &other);
    AudioMetaObject(const QUrl &url);
    virtual ~AudioMetaObject();

    inline AudioMetaObject &operator =(const AudioMetaObject &other) {
        if (this != &other)
            d.operator = (other.d);
        return *this;
    }

    bool operator == (const AudioMetaObject &other);
    inline bool operator != (const AudioMetaObject &other) {
        return !operator == (other);
    }

    ///
    /// \brief keyHash the primary key for audio meta data, each audio meta data had a unique hash
    /// this should be used for database searching
    /// \return
    ///
    static QString keyHash();

    static QString formatHash(const QString &value);
    static QString formatHash(const QString &path, const QString &name, quint64 size);

    QString hash() const;
    QString path() const;
    QString name() const;
    quint64 size() const;
    int mediaType() const;
    QString lyricsData() const;
    QUrl lyricsUri() const;

    void setMediaType(int arg);
    void setLyricsData(const QString &arg);
    void setLyricsUri(const QUrl &arg);

    AlbumMeta albumMeta() const;
    ArtistMeta artistMeta() const;
    CoverMeta coverMeta() const;
    TrackMeta trackMeta() const;

    void setAlbumMeta(const AlbumMeta &meta);
    void setArtistMeta(const ArtistMeta &meta);
    void setCoverMeta(const CoverMeta &meta);
    void setTrackMeta(const TrackMeta &meta);

    ///
    /// \brief uri
    /// \return 以file://开头的本地文件路径，或者网页地址
    ///
    QUrl uri() const;

    ///
    /// \brief 遍历所有属性，得到一个不为空的image地址
    /// \return
    ///
    QUrl queryImgUri() const;

    bool isEmpty() const;
    QJsonObject toObject() const;
    QByteArray toJson() const;
    QVariantMap toMap() const;
    static AudioMetaObject fromMap(const QVariantMap &map);
    static AudioMetaObject fromJson(const QByteArray &json);

private:
    class AudioMetaObjectPriv : public QSharedData
    {
    public:
        AudioMetaObjectPriv() {
            hash = QString();
            path = QString();
            name = QString();
            size = 0;
            mediaType = (int)PPCommon::MediaTypeLocalFile;
            lyricsData = QString();
            lyricsUri = QUrl();
            albumMeta = AlbumMeta();
            artistMeta = ArtistMeta();
            coverMeta = CoverMeta();
            trackMeta = TrackMeta();
        }

        QString hash;
        QString path;
        QString name;
        quint64 size;
        int mediaType;
        QString lyricsData;
        QUrl lyricsUri;

        AlbumMeta albumMeta;
        ArtistMeta artistMeta;
        CoverMeta coverMeta;
        TrackMeta trackMeta;
    };

    QSharedDataPointer<AudioMetaObjectPriv> d;
};
} //PhoenixPlayer
#endif // AUDIOMETAOBJECT_H
