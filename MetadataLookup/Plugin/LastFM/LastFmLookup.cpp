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

#include "SongMetaData.h"
#include "BaseNetworkLookup.h"
#include "Common.h"

namespace PhoenixPlayer{
class SongMetaData;
namespace MetadataLookup {
namespace LastFmLookup {

LastFmLookup::LastFmLookup(QObject *parent)
    :IMetadataLookup(parent)
{
    mNetworkLookup = new BaseNetworkLookup(this);

    connect (mNetworkLookup, &BaseNetworkLookup::failed,
             [this](const QUrl &requestedUrl, const QString &error) {
        Q_UNUSED(requestedUrl);
        qDebug()<<__FUNCTION__<<"lookup failed "<<error;
        emit lookupFailed ();
    });

    connect (mNetworkLookup, &BaseNetworkLookup::succeed,
             [this](const QUrl &requestedUrl, const QByteArray &replyData) {
        Q_UNUSED(requestedUrl);
        if (replyData.isEmpty ()) {
            qDebug()<<__FUNCTION__<<"replyData is empty ";
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

}

QString LastFmLookup::getPluginName()
{
    return QString(PLUGIN_NAME);
}

QString LastFmLookup::getPluginVersion()
{
    return QString(PLUGIN_VERSION);
}

QString LastFmLookup::getDescription()
{
    return QString(DESCRIPTION);
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
        return true;
    default:
        return false;
    }
}

//http://ws.audioscrobbler.com/2.0/?method=album.search&album=F.I.R&api_key=625fd47b3b685af19315cc3a1aa5920a&format=json
void LastFmLookup::lookup(SongMetaData *meta)
{
    QUrl url(API_BASE);
    QUrlQuery query;
    query.addQueryItem ("api_key", QString(API_KEY));
    query.addQueryItem ("format", QString("json"));

    switch (currentLookupFlag ()) {
    case LookupType::TypeAlbumImage:
    case LookupType::TypeAlbumDate:
    case LookupType::TypeAlbumDescription: {
        QString artist = meta->getMeta (Common::E_ArtistName).toString ();
        QString album = meta->getMeta (Common::E_AlbumName).toString ();
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
        QString artist = meta->getMeta (Common::E_ArtistName).toString ();
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
        QString artist = meta->getMeta (Common::E_ArtistName).toString ();
        QString track = meta->getMeta (Common::E_SongTitle).toString ();
        if (track.isEmpty ()) {
            track = meta->getMeta (Common::SongMetaTags::E_FileName).toString ();
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
        break;
    }
    url.setQuery (query);

    mNetworkLookup->setUrl (url.toString ());
    mNetworkLookup->setRequestType (BaseNetworkLookup::RequestGet);
    mNetworkLookup->startLookup ();
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
            emit lookupSucceed (v.toString ().toLocal8Bit ());
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
            emit lookupSucceed (v.toString ().toLocal8Bit ());
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
            emit lookupSucceed (v.toString ().toLocal8Bit ());
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
        foreach (QJsonValue value, array) {
            if (value.isUndefined ())
                continue;
            QJsonObject o = value.toObject ();
            if (o.isEmpty ())
                continue;
            if (o.value (QString("size")).toString () == QString(DEFAULT_IMAGE_SIZE_REGEXP)) {
                emit lookupSucceed (o.value (QString("#text")).toString ().toLocal8Bit ());
                found = true;
                break;
            }
        }
        if (!found)
            emit lookupFailed ();
    }


}

void LastFmLookup::parseArtisData(const QByteArray &qba)
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
            emit lookupSucceed (v.toString ().toLocal8Bit ());
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
        foreach (QJsonValue value, array) {
            if (value.isUndefined ())
                continue;
            QJsonObject o = value.toObject ();
            if (o.isEmpty ())
                continue;
            if (o.value (QString("size")).toString () == QString(DEFAULT_IMAGE_SIZE_REGEXP)) {
                emit lookupSucceed (o.value (QString("#text")).toString ().toLocal8Bit ());
                found = true;
                break;
            }
        }
        if (!found)
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


} //LastFmLookup
} //MetadataLookup
} //PhoenixPlayer
