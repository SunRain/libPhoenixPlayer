#ifndef AUDIOMETAOBJECT_H
#define AUDIOMETAOBJECT_H

#include <QSharedDataPointer>
#include <QVariant>
#include <QDate>
#include <QUrl>
class QStringList;
class QDate;
class QJsonObject;
namespace PhoenixPlayer {

class AlbumMetaPriv;
class AlbumMeta
{
public:
    explicit AlbumMeta();
    explicit AlbumMeta(const AlbumMeta &other);
    virtual ~AlbumMeta(){}

    AlbumMeta &operator =(const AlbumMeta &other) {
        if (*this == other)
            return *this;
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
    QVariant date() const;

    void setName(const QString &name);
    void setImgUri(const QUrl &uri);
    void setDescription(const QString &description);
    void setDate(const QVariant &date);

    QJsonObject toObject() const;
    QByteArray toJson() const;
    static AlbumMeta fromJson(const QByteArray &json);

private:
    QSharedDataPointer<AlbumMetaPriv> d;
};

class ArtistMetaPriv;
class ArtistMeta
{
public:
    explicit ArtistMeta();
    explicit ArtistMeta(const ArtistMeta &other);
    virtual ~ArtistMeta() {}
    ArtistMeta &operator =(const ArtistMeta &other) {
        if (*this == other)
            return *this;
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
    static ArtistMeta fromJson(const QByteArray &json);

private:
    QSharedDataPointer<ArtistMetaPriv> d;
};

class CoverMetaPriv;
class CoverMeta
{
public:
    explicit CoverMeta();
    explicit CoverMeta(const CoverMeta &other);
    virtual ~CoverMeta() {}
    CoverMeta &operator =(const CoverMeta &other) {
        if (*this == other)
            return *this;
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
class TrackMetaPriv;
class TrackMeta
{
public:
    explicit TrackMeta();
    explicit TrackMeta(const TrackMeta &other);
    virtual ~TrackMeta(){}

    TrackMeta &operator =(const TrackMeta &other) {
        if (*this == other)
            return *this;
        d.operator = (other.d);
        return *this;
    }
    bool operator == (const TrackMeta &other);
    inline bool operator != (const TrackMeta &other) {
        return !operator == (other);
    }

    QVariant bitRate() const;
    int duration() const;
    QString title() const;
    QString description() const;
    QVariant year() const;
    QVariant date() const;
    QVariant genre() const;
    QVariant sampleRate() const;
    QVariant userRating() const;

    void setBitRate(const QVariant &arg);
    void setDuration(int arg);
    void setTitle(const QString &arg);
    void setDescription(const QString &arg);
    void setYear(const QVariant &arg);
    void setDate(const QVariant &arg);
    void setGenre(const QVariant &arg);
    void setSampleRate(const QVariant &arg);
    void setUserRating(const QVariant &arg);

    QJsonObject toObject() const;
    QByteArray toJson() const;
    static TrackMeta fromJson(const QByteArray &json);

private:
    QSharedDataPointer<TrackMetaPriv> d;
};

class AudioMetaObjectPriv;
class AudioMetaObject
{
public:
    explicit AudioMetaObject();
    explicit AudioMetaObject(const QString &path, const QString &name, quint64 size);
    explicit AudioMetaObject(const AudioMetaObject &other);
    explicit AudioMetaObject(const QUrl &url);
    virtual ~AudioMetaObject();

    AudioMetaObject &operator =(const AudioMetaObject &other) {
        if (*this == other)
            return *this;
        d.operator = (other.d);
        return *this;
    }
    bool operator == (const AudioMetaObject &other);
    inline bool operator != (const AudioMetaObject &other) {
        return !operator == (other);
    }

    static QString keyHash() const;
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

    QJsonObject toObject() const;
    QByteArray toJson() const;
    static AudioMetaObject fromJson(const QByteArray &json);

private:
    QSharedDataPointer<AudioMetaObjectPriv> d;
};
} //PhoenixPlayer
#endif // AUDIOMETAOBJECT_H
