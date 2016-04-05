#include "LastFmLookup.h"

#include <QUrl>
#include <QByteArray>
#include <QDebug>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>

#include "AudioMetaObject.h"
#include "MetadataLookup/BaseNetworkLookup.h"
#include "Common.h"
#include "Settings.h"

namespace PhoenixPlayer{
namespace MetadataLookup {
namespace LastFmLookup {

LastFmLookup::LastFmLookup(QObject *parent)
    :IMetadataLookup(parent)
{
    m_networkLookup = new BaseNetworkLookup(this);
    Settings *setting = Settings::instance ();
    m_imageSizeValue = setting->getConfig (CONFIG_KEY, DEFAULT_IMAGE_SIZE_REGEXP);

    connect (m_networkLookup, &BaseNetworkLookup::failed,
             [&](const QUrl &requestedUrl, const QString &error) {
        Q_UNUSED(requestedUrl);
        qDebug()<<Q_FUNC_INFO<<"lookup failed "<<error;
        emit lookupFailed ();
    });

    connect (m_networkLookup, &BaseNetworkLookup::succeed,
             [&](const QUrl &requestedUrl, const QByteArray &replyData) {
        Q_UNUSED(requestedUrl);

        qDebug()<<Q_FUNC_INFO<<replyData;

        if (replyData.isEmpty ()) {
            qDebug()<<Q_FUNC_INFO<<"replyData is empty ";
            emit lookupFailed ();
            return;
        }
        switch (currentLookupFlag ()) {
        case LookupType::TypeAlbumDate:
        case LookupType::TypeAlbumDescription:
        case LookupType::TypeAlbumImage:
            parseAlbumData (replyData);
            break;
        case LookupType::TypeArtistDescription:
        case LookupType::TypeArtistImage:
            parseArtisData (replyData);
            break;
        case LookupType::TypeTrackDescription:
            parseSongDescription (replyData);
            break;
        default:
            emit lookupFailed ();
            break;
        }
    });

}

LastFmLookup::~LastFmLookup()
{
    qDebug()<<Q_FUNC_INFO;
}

bool LastFmLookup::supportLookup(IMetadataLookup::LookupType type)
{
    switch (type) {
    case LookupType::TypeAlbumDescription:
    case LookupType::TypeAlbumImage:
    case LookupType::TypeAlbumDate:
    case LookupType::TypeArtistDescription:
    case LookupType::TypeArtistImage:
    case LookupType::TypeTrackDescription:
    case LookupType::TypeUndefined:
    case LookupType::TypeLyrics:
        return true;
    default:
        return false;
    }
}

//http://ws.audioscrobbler.com/2.0/?method=album.search&album=F.I.R&api_key=625fd47b3b685af19315cc3a1aa5920a&format=json
void LastFmLookup::lookup(const AudioMetaObject &object)
{
    qDebug()<<"====> "<<Q_FUNC_INFO<<" <====";

#if 0
    emit lookupFailed ();
    return;
#endif

    QUrl url(API_BASE);
    QUrlQuery query;
    query.addQueryItem ("api_key", QString(API_KEY));
    query.addQueryItem ("format", QString("json"));

    switch (currentLookupFlag ()) {
    case LookupType::TypeAlbumImage:
    case LookupType::TypeAlbumDate:
    case LookupType::TypeAlbumDescription: {
        QString artist = formatStr (/*(*object)->artistMeta ()->name ()*/object.artistMeta ().name ());
        QString album = formatStr (/*(*object)->albumMeta ()->name ()*/object.albumMeta ().name ());
        if (!artist.isEmpty () && !album.isEmpty ()) {
            query.addQueryItem ("artist", artist);
            query.addQueryItem ("album", album);
            query.addQueryItem ("method", QString("album.getInfo"));
        } else {
            //TODO: last.fm alum info need artist name
            emit lookupFailed ();
            return;
        }
        break;
    }
    case LookupType::TypeArtistImage:
    case LookupType::TypeArtistDescription: {
        QString artist = formatStr (/*(*object)->artistMeta ()->name ()*/object.artistMeta ().name ());
        if (!artist.isEmpty ()) {
            query.addQueryItem ("artist", artist);
            query.addQueryItem ("method", QString("artist.getInfo"));
        } else {
            emit lookupFailed ();
            return;
        }
        break;
    }
    case LookupType::TypeTrackDescription: {
        QString artist = formatStr (/*(*object)->artistMeta ()->name ()*/object.artistMeta ().name ());
        QString track = formatStr (/*(*object)->trackMeta ()->title ()*/object.trackMeta ().title ());
        if (track.isEmpty ()) {
            track = (*object)->name ();
            //TODO: quick hack
            track = track.mid (0, track.indexOf ("."));
        }
        if (!artist.isEmpty () && !track.isEmpty ()) {
            query.addQueryItem ("artist", artist);
            query.addQueryItem ("track", track);
            query.addQueryItem ("method", QString("track.getInfo"));
        } else {
            //TODO: last.fm track info need artist name
            emit lookupFailed ();
            return;
        }
        break;
    }
    default:
        emit lookupFailed ();
        return;
    }
    url.setQuery (query);

    qDebug()<<Q_FUNC_INFO<<" URL "<<url <<" network is null "<<(m_networkLookup == nullptr);

    m_networkLookup->setUrl (url.toString ());
    m_networkLookup->setRequestType (BaseNetworkLookup::RequestGet);
    m_networkLookup->startLookup ();
}

void LastFmLookup::parseSongDescription(const QByteArray &qba)
{
    QJsonObject mainObj;
    if (!parseRootObject (mainObj, qba, QString("track"))) {
        emit lookupFailed ();
        return;
    }
    if (mainObj.isEmpty ()) {
        qDebug()<<"mainObj is isEmpty";
        emit lookupFailed ();
        return;
    }
    if (currentLookupFlag () == LookupType::TypeTrackDescription) {
        QJsonValue v = mainObj.value (QString("wiki"));
        if (v.isUndefined ()) {
            qDebug()<<"wiki is isEmpty";
            emit lookupFailed ();
            return;
        }
        QJsonObject o = v.toObject ();
        if (o.isEmpty ()) {
            qDebug()<<"wiki obj is isEmpty";
            emit lookupFailed ();
            return;
        }
        v = o.value (QString("content"));
        if (v.isUndefined ()) {
            qDebug()<<"wiki content is isUndefined";
            emit lookupFailed ();
        } else {
            QString str = v.toString ();
            if (str.isEmpty ())
                emit lookupFailed ();
            else
                emit lookupSucceed (str.toLocal8Bit ());
        }
    }
}

void LastFmLookup::parseAlbumData(const QByteArray &qba)
{
    QJsonObject mainObj;
    if (!parseRootObject (mainObj, qba, QString("album"))) {
        emit lookupFailed ();
        return;
    }
    if (mainObj.isEmpty ()) {
        qDebug()<<"mainObj is isEmpty";
        emit lookupFailed ();
        return;
    }
    if (currentLookupFlag () == LookupType::TypeAlbumDate) {
        QJsonValue v = mainObj.value (QString("releasedate"));
        if (v.isUndefined ()) {
            qDebug()<<"date is isEmpty";
            emit lookupFailed ();
        } else {
            QString str = v.toString ();
            if (str.isEmpty ())
                emit lookupFailed ();
            else
                emit lookupSucceed (str.toLocal8Bit ());
        }
    } else if (currentLookupFlag () == LookupType::TypeAlbumDescription) {
        QJsonValue v = mainObj.value (QString("wiki"));
        if (v.isUndefined ()) {
            qDebug()<<"wiki is isEmpty";
            emit lookupFailed ();
            return;
        }
        QJsonObject o = v.toObject ();
        if (o.isEmpty ()) {
            qDebug()<<"wiki obj is isEmpty";
            emit lookupFailed ();
            return;
        }
        v = o.value (QString("content"));
        if (v.isUndefined ()) {
            qDebug()<<"wiki content is isUndefined";
            emit lookupFailed ();
        } else {
            QString str = v.toString ();
            if (str.isEmpty ())
                emit lookupFailed ();
            else
                emit lookupSucceed (str.toLocal8Bit ());
        }
    } else if (currentLookupFlag () == LookupType::TypeAlbumImage) {
        QJsonValue v = mainObj.value (QString("image"));
        if (v.isUndefined () || !v.isArray ()) {
            qDebug()<<"image is isEmpty or no image array found";
            emit lookupFailed ();
            return;
        }
        QJsonArray array = v.toArray ();
        bool found = false;
        QString str;
        foreach (QJsonValue value, array) {
            if (value.isUndefined ())
                continue;
            QJsonObject o = value.toObject ();
            if (o.isEmpty ())
                continue;
            if (o.value (QString("size")).toString ()
                    == /*QString(DEFAULT_IMAGE_SIZE_REGEXP)*/m_imageSizeValue) {
                str = o.value (QString("#text")).toString ();
                found = true;
                break;
            }
        }
        if (found && !str.isEmpty ())
            emit lookupSucceed (str.toLocal8Bit ());
        else
            emit lookupFailed ();
    }
}

void LastFmLookup::parseArtisData(const QByteArray &qba)
{
    QJsonObject mainObj;
    if (!parseRootObject (mainObj, qba, QString("artist"))) {
        emit lookupFailed ();
        return;
    }
    if (mainObj.isEmpty ()) {
        qDebug()<<"mainObj is isEmpty";
        emit lookupFailed ();
        return;
    }
    if (currentLookupFlag () == LookupType::TypeArtistDescription) {
        QJsonValue v = mainObj.value (QString("bio"));
        if (v.isUndefined ()) {
            qDebug()<<"bio is isUndefined";
            emit lookupFailed ();
            return;
        }
        QJsonObject o = v.toObject ();
        if (o.isEmpty ()) {
            qDebug()<<"bio obj is isEmpty";
            emit lookupFailed ();
            return;
        }
        v = o.value (QString("content"));
        if (v.isUndefined ()) {
            qDebug()<<"content is isUndefined";
            emit lookupFailed ();
        } else {
            QString str = v.toString ();
            if (str.isEmpty ())
                emit lookupFailed ();
            else
                emit lookupSucceed (str.toLocal8Bit ());
        }
    } else if (currentLookupFlag () == LookupType::TypeArtistImage) {
        QJsonValue v = mainObj.value (QString("image"));
        if (v.isUndefined () || !v.isArray ()) {
            qDebug()<<"image is isEmpty or no image array found";
            emit lookupFailed ();
            return;
        }
        QJsonArray array = v.toArray ();
        bool found = false;
        QString str;
        foreach (QJsonValue value, array) {
            if (value.isUndefined ())
                continue;
            QJsonObject o = value.toObject ();
            if (o.isEmpty ())
                continue;
            if (o.value (QString("size")).toString ()
                    == /*QString(DEFAULT_IMAGE_SIZE_REGEXP)*/m_imageSizeValue) {
                str = o.value (QString("#text")).toString ();
//                emit lookupSucceed (o.value (QString("#text")).toString ().toLocal8Bit ());
                found = true;
                break;
            }
        }
        if (found && !str.isEmpty ())
            emit lookupSucceed (str.toLocal8Bit ());
        else
            emit lookupFailed ();
    }
}

bool LastFmLookup::parseRootObject(QJsonObject &out, const QByteArray &in, const QString &key)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson (in, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug()<<error.errorString ();
        return false;
    }
    if (!doc.isObject ()) {
        qDebug()<<"data is not json obj";
        return false;
    }
    QJsonObject docObj = doc.object ();
    QJsonValue mainValue = docObj.value (key);
    if (mainValue.isUndefined ()) {
        qDebug()<<"mainValue  for key "<<key<<" is undefined";
        return false;
    }
    out = mainValue.toObject ();
    return true;
}

QString LastFmLookup::formatStr(const QString &in)
{
    QString s = in;
    s = s.replace('#',"%23");
    s = s.replace('&',"%26");
    s = s.replace('+',"%2B");
    s = s.replace(' ',"+");
    return s;
}

} //LastFmLookup
} //MetadataLookup
} //PhoenixPlayer
