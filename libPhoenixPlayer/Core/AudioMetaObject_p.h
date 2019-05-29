#ifndef AUDIOMETAOBJECT_P_H
#define AUDIOMETAOBJECT_P_H

#include <QSharedDataPointer>
#include <QUrl>
#include <QVariant>
#include <QDate>

#include "libphoenixplayer_global.h"
#include "PPCommon.h"

namespace PhoenixPlayer {

class AlbumMetaPriv : public QSharedData
{
public:
    AlbumMetaPriv() {
        name = QString();
        imgUri = QUrl();
        description = QString();
        date = QString();
    }
    QString name;
    QUrl imgUri;
    QString description;
    QString date;
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
        bitRate = QString();
        duration = 0;
        title = QString();
        description = QString();
        year = QString();
        date = QString();
        genre = QString();
        sampleRate = QString();
        userRating = QString();
    }
    QString bitRate;
    int duration;
    QString title;
    QString description;
    QString year;
    QString date;
    QString genre;
    QString sampleRate;
    QString userRating;
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
