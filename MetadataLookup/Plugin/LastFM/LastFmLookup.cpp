#include "LastFmLookup.h"

#include <QUrl>
#include <QByteArray>
#include <QDebug>
#include <QUrlQuery>


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
        case LookupType::TypeAlbumDescription:
            parseAlbumDescription (replyData);
            break;
        case LookupType::TypeAlbumImage:
            parseAlbumImage (replyData);
            break;
        case LookupType::TypeArtistDescription:
            parseArtistDescription (replyData);
            break;
        case LookupType::TypeArtistImage:
            parseArtistImage (replyData);
            break;
        case LookupType::TypeSongDescription:
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
    case LookupType::TypeArtistDescription:
    case LookupType::TypeArtistImage:
    case LookupType::TypeSongDescription:
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
    case LookupType::TypeAlbumDescription: {
        QString artist = meta->getMeta (Common::E_ArtistName).toString ();
        QString album = meta->getMeta (Common.E_AlbumName).toSize ();
        if (!artist.isEmpty () && !album.isEmpty ()) {
            query.addQueryItem ("artist", artist);
            query.addQueryItem ("album", album);
            query.addQueryItem ("method", QString("album.getInfo"));
        } else {
            //TODO: last.fm alum info need artist name
            emit lookupFailed ();
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
        }
        break;
    }
    case LookupType::TypeSongDescription: {
        QString artist = meta->getMeta (Common::E_ArtistName).toString ();
        QString track = meta->getMeta (Common::E_SongTitle).toString ();
        if (track.isEmpty ()) {
            track = mMeta->getMeta (Common::SongMetaTags::E_FileName).toString ();
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
        }
        break;
    }
    default:
        emit lookupFailed ();
        break;
    }
    url.setQuery (query);

}

//void LastFmLookup::parseResult(QByteArray &qba)
//{
//    switch (currentLookupFlag ()) {
//    case LookupType::TypeAlbumDescription:
//        parseAlbumDescription (qba);
//        break;
//    case LookupType::TypeAlbumImage:
//        parseAlbumImage (qba);
//        break;
//    case LookupType::TypeArtistDescription:
//        parseArtistDescription (qba);
//        break;
//    case LookupType::TypeArtistImage:
//        parseArtistImage (qba);
//        break;
//    case LookupType::TypeSongDescription:
//        parseSongDescription (qba);
//        break;
//    default:
//        break;
//    }
//}


} //LastFmLookup
} //MetadataLookup
} //PhoenixPlayer
