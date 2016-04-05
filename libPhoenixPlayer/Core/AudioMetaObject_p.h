#ifndef AUDIOMETAOBJECT_P_H
#define AUDIOMETAOBJECT_P_H

#include <QSharedDataPointer>
#include <QUrl>
#include <QVariant>

#include "libphoenixplayer_global.h"
#include "Common.h"

namespace PhoenixPlayer {

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

//class AlbumMeta;
//class ArtistMeta;
//class CoverMeta;
//class TrackMeta;

//class AudioMetaObjectPriv : public QSharedData
//{
//public:
//    AudioMetaObjectPriv() {
//        hash = QString();
//        path = QString();
//        name = QString();
//        size = 0;
//        mediaType = (int)Common::MediaTypeLocalFile;
//        lyricsData = QString();
//        lyricsUri = QUrl();
//        albumMeta = AlbumMeta();
//        artistMeta = ArtistMeta();
//        coverMeta = CoverMeta();
//        trackMeta = TrackMeta();
//    }

//    QString hash;
//    QString path;
//    QString name;
//    quint64 size;
//    int mediaType;
//    QString lyricsData;
//    QUrl lyricsUri;

//    AlbumMeta albumMeta;
//    ArtistMeta artistMeta;
//    CoverMeta coverMeta;
//    TrackMeta trackMeta;
//};


} //PhoenixPlayer

#endif // AUDIOMETAOBJECT_P_H
