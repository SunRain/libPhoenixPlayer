#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

#include "Util.h"
#include "AudioMetaObject.h"
#include "Common.h"

namespace PhoenixPlayer {

#define KEY_NAME "NAME"
#define KEY_URI "URI"
#define KEY_DESCRIPTION "DESCRIPTION"
#define KEY_DATE "DATE"
#define KEY_SMALL_IMG "SMALL_URI"
#define KEY_MIDDLE_IMG "MIDDLE_URI"
#define KEY_LARGE_IMG "LARGE_URI"
#define KEY_BIT_RATE "BIT_RATE"
#define KEY_DURATION "DURATION"
#define KEY_TITLE "TITLE"
#define KEY_YEAR "YEAR"
#define KEY_GENRE "GENRE"
#define KEY_SAMPLE_RATE "SAMPLE_RATE"
#define KEY_USER_RATING "USER_RATING"
#define KEY_HASH "HASH"
#define KEY_PATH "PATH"
#define KEY_SIZE "SIZE"
#define KEY_MEDIA_TYPE "MEDIA_TYPE"
#define KEY_LYRICS_DATA "LYRICS_DATA"
#define KEY_LYRICS_URI "LYRICS_URI"
#define KEY_ALBUM_META "ALBUM_META"
#define KEY_ARTIST_META "ARTIST_META"
#define KEY_COVER_META "COVER_META"
#define KEY_TRACK_META "TRACK_META"

class AudioMetaObjectPriv : public QSharedData
{
public:
    AudioMetaObjectPriv() {
        hash = QString();
        path = QString();
        name = QString();
        size = 0;
        mediaType = (int)Common::MediaTypeLocalFile;
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

AudioMetaObject::AudioMetaObject()
    : d(new AudioMetaObjectPriv())
{
}

AudioMetaObject::AudioMetaObject(const QString &path, const QString &name, quint64 size)
    : d(new AudioMetaObjectPriv())
{
    d.data ()->path = path;
    d.data ()->name = name;
    d.data ()->size = size;
    d.data ()->hash = AudioMetaObject::formatHash(path, name, size);
}

AudioMetaObject::AudioMetaObject(const AudioMetaObject &other)
    : d(other.d)
{
}

AudioMetaObject::AudioMetaObject(const QUrl &url)
    : d(new AudioMetaObjectPriv())
{
    d.data ()->mediaType = (int)Common::MediaTypeUrl;
    d.data ()->path = url.toString ();
    d.data ()->hash = Util::calculateHash (url.toString ());
}

AudioMetaObject::~AudioMetaObject()
{

}

bool AudioMetaObject::operator ==(const AudioMetaObject &other)
{
    return d.data ()->albumMeta == other.d.data ()->albumMeta
            && d.data ()->artistMeta == other.d.data ()->artistMeta
            && d.data ()->coverMeta == other.d.data ()->coverMeta
            && d.data ()->hash == other.d.data ()->hash
            && d.data ()->lyricsData == other.d.data ()->lyricsData
            && d.data ()->lyricsUri == other.d.data ()->lyricsUri
            && d.data ()->mediaType == other.d.data ()->mediaType
            && d.data ()->name == other.d.data ()->name
            && d.data ()->path == other.d.data ()->path
            && d.data ()->size == other.d.data ()->size
            && d.data ()->trackMeta == other.d.data ()->trackMeta;
}

QString AudioMetaObject::keyHash() const
{
    return QString(KEY_HASH);
}
QString AudioMetaObject::formatHash(const QString &path, const QString &name, quint64 size)
{
    return Util::calculateHash (QString("%1/%2-%3").arg (path).arg (name).arg (size));
}

QString AudioMetaObject::hash() const
{
    return d.data ()->hash;
}

QString AudioMetaObject::path() const
{
    return d.data ()->path;
}

QString AudioMetaObject::name() const
{
    return d.data ()->name;
}

quint64 AudioMetaObject::size() const
{
    return d.data ()->size;
}

int AudioMetaObject::mediaType() const
{
    return d.data ()->mediaType;
}

QString AudioMetaObject::lyricsData() const
{
    return d.data ()->lyricsData;
}

QUrl AudioMetaObject::lyricsUri() const
{
    return d.data ()->lyricsUri;
}

void AudioMetaObject::setMediaType(int arg)
{
    d.data ()->mediaType = arg;
}

void AudioMetaObject::setLyricsData(const QString &arg)
{
    d.data ()->lyricsData = arg;
}

void AudioMetaObject::setLyricsUri(const QUrl &arg)
{
    d.data ()->lyricsUri = arg;
}

AlbumMeta AudioMetaObject::albumMeta() const
{
    return d.data ()->albumMeta;
}

ArtistMeta AudioMetaObject::artistMeta() const
{
    return d.data ()->artistMeta;
}

CoverMeta AudioMetaObject::coverMeta() const
{
    return d.data ()->coverMeta;
}

TrackMeta AudioMetaObject::trackMeta() const
{
    return d.data ()->trackMeta;
}

void AudioMetaObject::setAlbumMeta(const AlbumMeta &meta)
{
    d.data ()->albumMeta = meta;
}

void AudioMetaObject::setArtistMeta(const ArtistMeta &meta)
{
    d.data ()->artistMeta = meta;
}

void AudioMetaObject::setCoverMeta(const CoverMeta &meta)
{
    d.data ()->coverMeta = meta;
}

void AudioMetaObject::setTrackMeta(const TrackMeta &meta)
{
    d.data ()->trackMeta = meta;
}

QUrl AudioMetaObject::uri() const
{
    if (d.data ()->mediaType == (int)Common::MediaTypeLocalFile) {
        if (!d.data ()->path.isEmpty () && !d.data ()->name.isEmpty ())
            //TODO file:// or file:/ ? or no file:/ (file://) ?
            //FIXME should fix path like ../path/to/file
            return QUrl::fromLocalFile (QString("%1/%2").arg (d.data ()->path).arg (d.data ()->name));
    } else {
        if (!d.data ()->path.isEmpty ())
            return QUrl(d.data ()->path);
        else if (!d.data ()->name.isEmpty ())
            return QUrl(d.data ()->name);
    }
    return QUrl();
}

QUrl AudioMetaObject::queryImgUri() const
{
    if (!coverMeta ().middleUri ().isEmpty ())
        return coverMeta ().middleUri ();
    if (!coverMeta ().largeUri ().isEmpty ())
        return coverMeta ().largeUri ();
    if (!coverMeta ().smallUri ().isEmpty ())
        return coverMeta ().smallUri ();
    if (!artistMeta ().imgUri ().isEmpty ())
        return artistMeta ().imgUri ();
    if (!albumMeta ().imgUri ().isEmpty ())
        return albumMeta ().imgUri ();
    return QUrl();
}

bool AudioMetaObject::isEmpty() const
{
    return d.data ()->hash.isEmpty ();
}

QJsonObject AudioMetaObject::toObject() const
{
    QJsonObject o;
    o.insert (KEY_HASH, d.data ()->hash);
    o.insert (KEY_PATH, d.data ()->path);
    o.insert (KEY_NAME, d.data ()->name);
    o.insert (KEY_SIZE, d.data ()->size);
    o.insert (KEY_MEDIA_TYPE, d.data ()->mediaType);
    o.insert (KEY_LYRICS_DATA, d.data ()->lyricsData);
    o.insert (KEY_LYRICS_URI, d.data ()->lyricsUri);
    o.insert (KEY_ALBUM_META, d.data ()->albumMeta.toJson ());
    o.insert (KEY_ARTIST_META, d.data ()->artistMeta.toJson ());
    o.insert (KEY_COVER_META, d.data ()->coverMeta.toJson ());
    o.insert (KEY_TRACK_META, d.data ()->trackMeta.toJson ());
    return o;
}

QByteArray AudioMetaObject::toJson() const
{
    QJsonDocument doc(toObject ());
    return doc.toJson ();
}

AudioMetaObject AudioMetaObject::fromJson(const QByteArray &json)
{
    AudioMetaObject meta;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson (json, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug()<<Q_FUNC_INFO<<"Parse main json content error";
        return meta;
    }
    if (doc.isEmpty () || doc.isNull () || !doc.isObject ())
        return meta;
    QJsonObject o = doc.object ();
    if (o.isEmpty ())
        return meta;

    QString name = o.value (KEY_NAME).toString ();
    QString path = o.value (KEY_PATH).toString ();
    QString size = o.value (KEY_SIZE).toInt ();
    AudioMetaObject m(path, name, size);
    meta = m;
    AlbumMeta al = AlbumMeta::fromJson (o.value (KEY_ALBUM_META).toString ().toUtf8 ());
    ArtistMeta ar = ArtistMeta::fromJson (o.value (KEY_ARTIST_META).toString ().toUtf8 ());
    CoverMeta co = CoverMeta::fromJson (o.value (KEY_COVER_META).toString ().toUtf8 ());
    TrackMeta tr = TrackMeta::fromJson (o.value (KEY_TRACK_META).toString ().toUtf8 ());
    meta.setAlbumMeta (al);
    meta.setArtistMeta (ar);
    meta.setCoverMeta (co);
    meta.setTrackMeta (tr);
    meta.setLyricsData (o.value (KEY_LYRICS_DATA).toString ());
    meta.setLyricsUri (QUrl(o.value (KEY_LYRICS_URI).toString ()));
    meta.setMediaType (o.value (KEY_MEDIA_TYPE).toInt ());

    return meta;
}

class AlbumMetaPriv : public QSharedData
{
public:
    AlbumMetaPriv() {
        name = QString();
        imgUri = QUrl();
        description = QString();
        date = QDate::currentDate ();
    }
    QString name;
    QUrl imgUri;
    QString description;
    QVariant date;
};

class ArtistMetaPriv : public QSharedData
{
public:
    ArtistMetaPriv() {
        name = QString();
        imgUri = QUrl();
        description = QString();
    }
    QString name;
    QUrl imgUri;
    QString description;
};

class CoverMetaPriv : public QSharedData
{
public:
    CoverMetaPriv() {
        smallUri = QUrl();
        middleUri = QUrl();
        largeUri = QUrl();
    }
    QUrl smallUri;
    QUrl middleUri;
    QUrl largeUri;
};

class TrackMetaPriv : public QSharedData
{
public:
    TrackMetaPriv() {
        bitRate = QVariant();
        duration = 0;
        title = QString();
        description = QString();
        year = QVariant();
        date = QVariant();
        genre = QVariant();
        sampleRate = QVariant();
        userRating = QVariant();
    }
    QVariant bitRate;
    int duration;
    QString title;
    QString description;
    QVariant year;
    QVariant date;
    QVariant genre;
    QVariant sampleRate;
    QVariant userRating;
};

AlbumMeta::AlbumMeta()
    : d(new AlbumMetaPriv())
{
}

AlbumMeta::AlbumMeta(const AlbumMeta &other)
    : d(other.d)
{
}

bool AlbumMeta::operator ==(const AlbumMeta &other)
{
    return d.data ()->date == other.d.data ()->date
            && d.data ()->description == other.d.data ()->description
            && d.data ()->imgUri == other.d.data ()->imgUri
            && d.data ()->name == other.d.data ()->name;
}

QString AlbumMeta::name() const
{
    return d.data ()->name;
}

QUrl AlbumMeta::imgUri() const
{
    return d.data ()->imgUri;
}

QString AlbumMeta::description() const
{
    return d.data ()->description;
}

QVariant AlbumMeta::date() const
{
    return d.data ()->date;
}

void AlbumMeta::setName(const QString &name)
{
    d.data ()->name = name;
}

void AlbumMeta::setImgUri(const QUrl &uri)
{
    d.data ()->imgUri = uri;
}

void AlbumMeta::setDescription(const QString &description)
{
    d.data ()->description = description;
}

void AlbumMeta::setDate(const QVariant &date)
{
    d.data ()->date = date;
}

QJsonObject AlbumMeta::toObject() const
{
    QJsonObject o;
    o.insert (KEY_NAME, d.data ()->name);
    o.insert (KEY_URI, d.data ()->imgUri);
    o.insert (KEY_DESCRIPTION, d.data ()->description);
    o.insert (KEY_DATE, d.data ()->date);
    return o;
}

QByteArray AlbumMeta::toJson() const
{
    QJsonDocument doc(toObject ());
    return doc.toJson ();
}

AlbumMeta AlbumMeta::fromJson(const QByteArray &json)
{
    AlbumMeta meta;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson (json, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug()<<Q_FUNC_INFO<<"Parse main json content error";
        return meta;
    }
    if (doc.isEmpty () || doc.isNull () || !doc.isObject ())
        return meta;
    QJsonObject o = doc.object ();
    if (o.isEmpty ())
        return meta;
    meta.setDate (o.value (KEY_DATE).toVariant ());
    meta.setDescription (o.value (KEY_DESCRIPTION).toString ());
    meta.setImgUri (QUrl(o.value (KEY_URI).toString ()));
    meta.setName (o.value (KEY_NAME).toString ());
    return meta;
}

ArtistMeta::ArtistMeta()
    : d(new ArtistMetaPriv())
{
}

ArtistMeta::ArtistMeta(const ArtistMeta &other)
    : d(other.d)
{
}

bool ArtistMeta::operator ==(const ArtistMeta &other)
{
    return d.data ()->description == other.d.data ()->description
            && d.data ()->imgUri == other.d.data ()->imgUri
            && d.data ()->name == other.d.data ()->name;
}

QString ArtistMeta::name() const
{
    return d.data ()->name;
}

QUrl ArtistMeta::imgUri() const
{
    return d.data ()->imgUri;
}

QString ArtistMeta::description() const
{
    return d.data ()->description;
}

void ArtistMeta::setName(const QString &name)
{
    d.data ()->name = name;
}

void ArtistMeta::setImgUri(const QUrl &uri)
{
    d.data ()->imgUri = uri;
}

void ArtistMeta::setDescription(const QString &description)
{
    d.data ()->description = description;
}

QJsonObject ArtistMeta::toObject() const
{
    QJsonObject o;
    o.insert (KEY_NAME, d.data ()->name);
    o.insert (KEY_URI, d.data ()->imgUri);
    o.insert (KEY_DESCRIPTION, d.data ()->description);
    return o;
}

QByteArray ArtistMeta::toJson() const
{
    QJsonDocument doc(toObject ());
    return doc.toJson ();
}

ArtistMeta ArtistMeta::fromJson(const QByteArray &json)
{
    ArtistMeta meta;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson (json, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug()<<Q_FUNC_INFO<<"Parse main json content error";
        return meta;
    }
    if (doc.isEmpty () || doc.isNull () || !doc.isObject ())
        return meta;
    QJsonObject o = doc.object ();
    if (o.isEmpty ())
        return meta;
    meta.setDescription (o.value (KEY_DESCRIPTION).toString ());
    meta.setImgUri (QUrl(o.value (KEY_URI).toString ()));
    meta.setName (o.value (KEY_NAME).toString ());
    return meta;
}

CoverMeta::CoverMeta()
    : d(new CoverMetaPriv())
{
}

CoverMeta::CoverMeta(const CoverMeta &other)
    : d(other.d)
{
}

bool CoverMeta::operator ==(const CoverMeta &other)
{
    return d.data ()->largeUri == other.d.data ()->largeUri
            && d.data ()->middleUri == other.d.data ()->middleUri
            && d.data ()->smallUri == other.d.data ()->smallUri;
}

QUrl CoverMeta::smallUri() const
{
    return d.data ()->smallUri;
}

QUrl CoverMeta::middleUri() const
{
    return d.data ()->middleUri;
}

QUrl CoverMeta::largeUri() const
{
    return d.data ()->largeUri;
}

void CoverMeta::setSmallUri(const QUrl &small)
{
    d.data ()->smallUri = small;
}

void CoverMeta::setMiddleUri(const QUrl &middle)
{
    d.data ()->middleUri = middle;
}

void CoverMeta::setLargeUri(const QUrl &large)
{
    d.data ()->largeUri = large;
}

QJsonObject CoverMeta::toObject() const
{
    QJsonObject o;
    o.insert (KEY_LARGE_IMG, d.data ()->largeUri);
    o.insert (KEY_MIDDLE_IMG, d.data ()->middleUri);
    o.insert (KEY_SMALL_IMG, d.data ()->smallUri);
    return o;
}

QByteArray CoverMeta::toJson() const
{
    QJsonDocument doc(toObject ());
    return doc.toJson ();
}

CoverMeta CoverMeta::fromJson(const QByteArray &json)
{
    CoverMeta meta;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson (json, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug()<<Q_FUNC_INFO<<"Parse main json content error";
        return meta;
    }
    if (doc.isEmpty () || doc.isNull () || !doc.isObject ())
        return meta;
    QJsonObject o = doc.object ();
    if (o.isEmpty ())
        return meta;
    meta.setLargeUri (QUrl(o.value (KEY_LARGE_IMG).toString ()));
    meta.setMiddleUri (QUrl(o.value (KEY_MIDDLE_IMG).toString ()));
    meta.setSmallUri (QUrl(o.value (KEY_SMALL_IMG).toString ()));
    return meta;
}

TrackMeta::TrackMeta()
    : d(new TrackMetaPriv())
{

}

TrackMeta::TrackMeta(const TrackMeta &other)
    : d(other.d)
{

}

bool TrackMeta::operator ==(const TrackMeta &other)
{
    return d.data ()->bitRate == other.d.data ()->bitRate
            && d.data ()->date == other.d.data ()->date
            && d.data ()->description == other.d.data ()->description
            && d.data ()->duration == other.d.data ()->duration
            && d.data ()->genre == other.d.data ()->genre
            && d.data ()->sampleRate == other.d.data ()->sampleRate
            && d.data ()->title == other.d.data ()->title
            && d.data ()->userRating == other.d.data ()->userRating
            && d.data ()->year == other.d.data ()->year;
}

QVariant TrackMeta::bitRate() const
{
    return d.data ()->bitRate;
}

int TrackMeta::duration() const
{
    return d.data ()->duration;
}

QString TrackMeta::title() const
{
    return d.data ()->title;
}

QString TrackMeta::description() const
{
    return d.data ()->description;
}

QVariant TrackMeta::year() const
{
    return d.data ()->year;
}

QVariant TrackMeta::date() const
{
    return d.data ()->date;
}

QVariant TrackMeta::genre() const
{
    return d.data ()->genre;
}

QVariant TrackMeta::sampleRate() const
{
    return d.data ()->sampleRate;
}

QVariant TrackMeta::userRating() const
{
    return d.data ()->userRating;
}

void TrackMeta::setBitRate(const QVariant &arg)
{
    d.data ()->bitRate = arg;
}

void TrackMeta::setDuration(int arg)
{
    d.data ()->duration = arg;
}

void TrackMeta::setTitle(const QString &arg)
{
    d.data ()->title = arg;
}

void TrackMeta::setDescription(const QString &arg)
{
    d.data ()->description = arg;
}

void TrackMeta::setYear(const QVariant &arg)
{
    d.data ()->year = arg;
}

void TrackMeta::setDate(const QVariant &arg)
{
    d.data ()->date = arg;
}

void TrackMeta::setGenre(const QVariant &arg)
{
    d.data ()->genre = arg;
}

void TrackMeta::setSampleRate(const QVariant &arg)
{
    d.data ()->sampleRate = arg;
}

void TrackMeta::setUserRating(const QVariant &arg)
{
    d.data ()->userRating = arg;
}

QJsonObject TrackMeta::toObject() const
{
    QJsonObject o;
    o.insert (KEY_BIT_RATE, d.data ()->bitRate);
    o.insert (KEY_DATE, d.data ()->date);
    o.insert (KEY_DESCRIPTION, d.data ()->description);
    o.insert (KEY_DURATION, d.data ()->duration);
    o.insert (KEY_GENRE, d.data ()->genre);
    o.insert (KEY_SAMPLE_RATE, d.data ()->sampleRate);
    o.insert (KEY_TITLE, d.data ()->title);
    o.insert (KEY_USER_RATING, d.data ()->userRating);
    o.insert (KEY_YEAR, d.data ()->year);
    return o;
}

QByteArray TrackMeta::toJson() const
{
    QJsonDocument doc(toObject ());
    return doc.toJson ();
}

TrackMeta TrackMeta::fromJson(const QByteArray &json)
{
    TrackMeta meta;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson (json, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug()<<Q_FUNC_INFO<<"Parse main json content error";
        return meta;
    }
    if (doc.isEmpty () || doc.isNull () || !doc.isObject ())
        return meta;
    QJsonObject o = doc.object ();
    if (o.isEmpty ())
        return meta;
    meta.setBitRate (o.value (KEY_BIT_RATE).toVariant ());
    meta.setDate (o.value (KEY_DATE).toVariant ());
    meta.setDescription (o.value (KEY_DESCRIPTION).toString ());
    meta.setDuration (o.value (KEY_DURATION).toInt ());
    meta.setGenre (o.value (KEY_GENRE).toVariant ());
    meta.setSampleRate (o.value (KEY_SAMPLE_RATE).toVariant ());
    meta.setTitle (o.value (KEY_TITLE).toString ());
    meta.setUserRating (o.value (KEY_USER_RATING).toVariant ());
    meta.setYear (o.value (KEY_YEAR).toVariant ());
    return meta;
}

} //PhoenixPlayer
