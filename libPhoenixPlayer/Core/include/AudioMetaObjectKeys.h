#ifndef AUDIOMETAOBJECTKEYS_H
#define AUDIOMETAOBJECTKEYS_H

#include <QString>
#include <QObject>
#include <QDebug>
#ifdef WITH_QML_LIB
#include <QtQml>
#endif
//#include <QQuickWindow>

#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {
// keys for AudioMetaObject
const static char *KEY_NAME             = "NAME";
const static char *KEY_URI              = "URI";
const static char *KEY_DESCRIPTION      = "DESCRIPTION";
const static char *KEY_DATE             = "DATE";
const static char *KEY_SMALL_IMG        = "SMALL_URI";
const static char *KEY_MIDDLE_IMG       = "MIDDLE_URI";
const static char *KEY_LARGE_IMG        = "LARGE_URI";
const static char *KEY_BIT_RATE         = "BIT_RATE";
const static char *KEY_DURATION         = "DURATION";
const static char *KEY_TITLE            = "TITLE";
const static char *KEY_YEAR             = "YEAR";
const static char *KEY_GENRE            = "GENRE";
const static char *KEY_SAMPLE_RATE      = "SAMPLE_RATE";
const static char *KEY_USER_RATING      = "USER_RATING";
const static char *KEY_HASH             = "HASH";
const static char *KEY_PATH             = "PATH";
const static char *KEY_SIZE             = "SIZE";
const static char *KEY_MEDIA_TYPE       = "MEDIA_TYPE";
const static char *KEY_LYRICS_DATA      = "LYRICS_DATA";
const static char *KEY_LYRICS_URI       = "LYRICS_URI";
const static char *KEY_ALBUM_META       = "ALBUM_META";
const static char *KEY_ARTIST_META      = "ARTIST_META";
const static char *KEY_COVER_META       = "COVER_META";
const static char *KEY_TRACK_META       = "TRACK_META";

class LIBPHOENIXPLAYER_EXPORT AudioMetaObjectKeyName : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString KeyName READ KeyName CONSTANT)
    Q_PROPERTY(QString keyUri READ keyUri CONSTANT)
    Q_PROPERTY(QString KeyDescription READ KeyDescription CONSTANT)
    Q_PROPERTY(QString KeyDate READ KeyDate CONSTANT)
    Q_PROPERTY(QString KeySmallImg READ KeySmallImg CONSTANT)
    Q_PROPERTY(QString KeyMiddleImg READ KeyMiddleImg CONSTANT)
    Q_PROPERTY(QString KeyLargeImg READ KeyLargeImg CONSTANT)
    Q_PROPERTY(QString KeyBitRate READ KeyBitRate CONSTANT)
    Q_PROPERTY(QString KeyDuration READ KeyDuration CONSTANT)
    Q_PROPERTY(QString KeyTitle READ KeyTitle CONSTANT)
    Q_PROPERTY(QString KeyYear READ KeyYear CONSTANT)
    Q_PROPERTY(QString KeyGenre READ KeyGenre CONSTANT)
    Q_PROPERTY(QString KeySampleRate READ KeySampleRate CONSTANT)
    Q_PROPERTY(QString KeyUserRating READ KeyUserRating CONSTANT)
    Q_PROPERTY(QString KeyHash READ KeyHash CONSTANT)
    Q_PROPERTY(QString KeyPath READ KeyPath CONSTANT)
    Q_PROPERTY(QString KeySize READ KeySize CONSTANT)
    Q_PROPERTY(QString KeyMediaType READ KeyMediaType CONSTANT)
    Q_PROPERTY(QString KeyLyricsData READ KeyLyricsData CONSTANT)
    Q_PROPERTY(QString KeyLyricsUri READ KeyLyricsUri CONSTANT)
    Q_PROPERTY(QString KeyAlbumMeta READ KeyAlbumMeta CONSTANT)
    Q_PROPERTY(QString KeyArtistMeta READ KeyArtistMeta CONSTANT)
    Q_PROPERTY(QString KeyCoverMeta READ KeyCoverMeta CONSTANT)
    Q_PROPERTY(QString KeyTrackMeta READ KeyTrackMeta CONSTANT)
public:
    AudioMetaObjectKeyName(QObject *parent = 0)
        : QObject(parent)
    {
    }
    virtual ~AudioMetaObjectKeyName() {}

#ifdef WITH_QML_LIB
    static QObject *qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new AudioMetaObjectKeyName();
    }
#endif

    QString KeyName() const
    {
        return KEY_NAME;
    }
    QString keyUri() const
    {
        return KEY_URI;
    }
    QString KeyDescription() const
    {
        return KEY_DESCRIPTION;
    }
    QString KeyDate() const
    {
        return KEY_DATE;
    }
    QString KeySmallImg() const
    {
        return KEY_SMALL_IMG;
    }
    QString KeyMiddleImg() const
    {
        return KEY_MIDDLE_IMG;
    }
    QString KeyLargeImg() const
    {
        return KEY_LARGE_IMG;
    }
    QString KeyBitRate() const
    {
        return KEY_BIT_RATE;
    }
    QString KeyDuration() const
    {
        return KEY_DURATION;
    }
    QString KeyTitle() const
    {
        return KEY_TITLE;
    }
    QString KeyYear() const
    {
        return KEY_YEAR;
    }
    QString KeyGenre() const
    {
        return KEY_GENRE;
    }
    QString KeySampleRate() const
    {
        return KEY_SAMPLE_RATE;
    }
    QString KeyUserRating() const
    {
        return KEY_USER_RATING;
    }
    QString KeyHash() const
    {
        return KEY_HASH;
    }
    QString KeyPath() const
    {
        return KEY_PATH;
    }
    QString KeySize() const
    {
        return KEY_SIZE;
    }
    QString KeyMediaType() const
    {
        return KEY_MEDIA_TYPE;
    }
    QString KeyLyricsData() const
    {
        return KEY_LYRICS_DATA;
    }
    QString KeyLyricsUri() const
    {
        return KEY_LYRICS_URI;
    }
    QString KeyAlbumMeta() const
    {
        return KEY_ALBUM_META;
    }
    QString KeyArtistMeta() const
    {
        return KEY_ARTIST_META;
    }
    QString KeyCoverMeta() const
    {
        return KEY_COVER_META;
    }
    QString KeyTrackMeta() const
    {
        return KEY_TRACK_META;
    }
};

}

#endif // AUDIOMETAOBJECTKEYS_H
