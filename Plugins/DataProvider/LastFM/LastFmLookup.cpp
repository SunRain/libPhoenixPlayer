#include "LastFmLookup.h"

#include <QUrl>
#include <QByteArray>
#include <QDebug>
#include <QTimer>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>

#include "AudioMetaObject.h"
#include "PPCommon.h"
#include "PPSettings.h"

#include "QCNetworkAccessManager.h"
#include "QCNetworkSyncReply.h"
#include "QCNetworkRequest.h"

namespace PhoenixPlayer{
namespace DataProvider {
namespace LastFmLookup {

using namespace QCurl;

const static char *LASTFMAPIKEY     = "48570d887cca45f05f8094dca4f79c0d";
const static char *API_BASE         = "http://ws.audioscrobbler.com/2.0";
const static char *API_KEY          = "625fd47b3b685af19315cc3a1aa5920a";

/*
 *  macro based on last.fm json result
 *  should use a configuration later
 *
 */
const static char *IMAGE_SIZE_REGEXP_LARGE          = "large";
const static char *IMAGE_SIZE_REGEXP_MEDIA          = "medium";
const static char *IMAGE_SIZE_REGEXP_SMALL          = "small";
const static char *IMAGE_SIZE_REGEXP_EXTRA_LARGE    = "extralarge";
const static char *IMAGE_SIZE_REGEXP_MEGA           = "mega";

const static char *CONFIG_KEY = "PluginLastFmLookup/LastFM_Image_Size";

const static int TIMER_INTERVAL_MS = 10000; //10s

#define DEFAULT_IMAGE_SIZE_REGEXP IMAGE_SIZE_REGEXP_MEDIA

LastFmLookup::LastFmLookup(QObject *parent)
    :IMetadataLookup(parent)
{
    PPSettings setting;
    m_imageSizeValue = setting.getConfig(CONFIG_KEY, DEFAULT_IMAGE_SIZE_REGEXP);

    m_network = new QCNetworkAccessManager(this);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);

//    connect (m_networkLookup, &BaseNetworkLookup::failed,
//             [&](const QUrl &requestedUrl, const QString &error) {
//        Q_UNUSED(requestedUrl);
//        qDebug()<<Q_FUNC_INFO<<"lookup failed "<<error;
//        emit lookupFailed ();
//    });

//    connect (m_networkLookup, &BaseNetworkLookup::succeed,
//             [&](const QUrl &requestedUrl, const QByteArray &replyData) {
//        Q_UNUSED(requestedUrl);

//        qDebug()<<Q_FUNC_INFO<<replyData;

//        if (replyData.isEmpty ()) {
//            qDebug()<<Q_FUNC_INFO<<"replyData is empty ";
//            emit lookupFailed ();
//            return;
//        }
//        switch (currentLookupFlag ()) {
//        case LookupType::TypeAlbumDate:
//        case LookupType::TypeAlbumDescription:
//        case LookupType::TypeAlbumImage:
//            parseAlbumData (replyData);
//            break;
//        case LookupType::TypeArtistDescription:
//        case LookupType::TypeArtistImage:
//            parseArtisData (replyData);
//            break;
//        case LookupType::TypeTrackDescription:
//            parseSongDescription (replyData);
//            break;
//        default:
//            emit lookupFailed ();
//            break;
//        }
//    });

}

LastFmLookup::~LastFmLookup()
{
    qDebug()<<Q_FUNC_INFO;
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
        m_timer->deleteLater();
        m_timer = Q_NULLPTR;
    }

    if (m_network) {
        m_network->deleteLater();
        m_network = Q_NULLPTR;
    }
}

PluginProperty LastFmLookup::property() const
{
    return PluginProperty("LastFmLookup",
                          "1.0",
                          "Track metadata lookup by last.fm",
                          false,
                          true);

}

bool LastFmLookup::support(IDataProvider::SupportedTypes type) const
{
    if ((type & SupportLookupAlbumDescription) == type) {
        return true;
    } else if ((type & SupportLookupAlbumImage) == type) {
        return true;
    } else if ((type & SupportLookupAlbumDate) == type) {
        return true;
    } else if ((type & SupportLookupArtistImage) == type) {
        return true;
    } else if ((type & SupportLookupArtistDescription) == type) {
        return true;
    } else if ((type & SupportLookupTrackDescription) == type) {
        return true;
    }
//    if ((type & SupportLookupLyrics) == type) {
//        return true;
//    }
    return false;
}

QByteArray LastFmLookup::lookup(const AudioMetaObject &object, IDataProvider::SupportedType type, QString *errorMsg) const
{
    if (object.isHashEmpty()) {
        if (errorMsg) {
            errorMsg->append("AudioMetaObject hash is empty!");
        }
        return QByteArray();
    }
    if (!support(type)) {
        if (errorMsg) {
            errorMsg->append(QString("Current type [%1] not supported").arg(enumToStr("SupportedType", type)));
        }
        return QByteArray();
    }

    if (m_timer->isActive()) {
        m_timer->disconnect();
        m_timer->stop();
    }

    QUrl url(API_BASE);
    QUrlQuery query;
    query.addQueryItem ("api_key", QString(API_KEY));
    query.addQueryItem ("format", QString("json"));

    switch (type) {
    case SupportLookupAlbumImage:
    case SupportLookupAlbumDate:
    case SupportLookupAlbumDescription:
    {
        const QString artist = formatStr(object.artistMeta().name());
        const QString album = formatStr(object.albumMeta().name());
        if (!artist.isEmpty() && !album.isEmpty ()) {
            query.addQueryItem("artist", artist);
            query.addQueryItem("album", album);
            query.addQueryItem("method", QString("album.getInfo"));
        } else {
            //TODO: last.fm alum info need artist name
            setErrorMsg(errorMsg, "Lookup album info need aritis and album name, but one (or all) of them is empty !");
            return QByteArray();
        }
        break;
    }
    case SupportLookupArtistImage:
    case SupportLookupArtistDescription:
    {
        QString artist = formatStr(object.artistMeta().name());
        if (!artist.isEmpty()) {
            query.addQueryItem("artist", artist);
            query.addQueryItem("method", QString("artist.getInfo"));
        } else {
            setErrorMsg(errorMsg, "Artist name is empty !");
            return QByteArray();
        }
        break;
    }
    case SupportLookupTrackDescription:
    {
        QString artist = formatStr(object.artistMeta().name());
        QString track = formatStr(object.trackMeta().title());
        if (track.isEmpty()) {
            track = object.name ();
            //TODO: quick hack
            track = track.mid (0, track.indexOf ("."));
        }
        if (!artist.isEmpty() && !track.isEmpty()) {
            query.addQueryItem("artist", artist);
            query.addQueryItem("track", track);
            query.addQueryItem("method", QString("track.getInfo"));
        } else {
            //TODO: last.fm track info need artist name
            setErrorMsg(errorMsg, "Lookup track dscription need aritis and track name, but one (or all) of them is empty !");
            return QByteArray();
        }
        break;
    }
    default:
    {
        setErrorMsg(errorMsg, "Can't support current lookup type");
        return QByteArray();
    }
    }

    url.setQuery(query);
    QCNetworkRequest request(url);
    this->setRequestHeader(&request);

    QByteArray replyData = networkCall(request, errorMsg);

    if (replyData.isEmpty()) {
        setErrorMsg(errorMsg, "Empty reply data!");
        return QByteArray();
    }

    switch (type) {
    case SupportLookupAlbumImage:
    case SupportLookupAlbumDate:
    case SupportLookupAlbumDescription:
    {
        return parseAlbumData(replyData, type);
    }
    case SupportLookupArtistImage:
    case SupportLookupArtistDescription:
    {
        return parseArtisData(replyData, type);
    }
    case SupportLookupTrackDescription:
    {
        return parseSongDescription(replyData);
    }
    default:
    {
        setErrorMsg(errorMsg, "Can't support current lookup type");
        return QByteArray();
    }
    }
}

//bool LastFmLookup::supportLookup(IMetadataLookup::LookupType type)
//{
//    switch (type) {
//    case LookupType::TypeAlbumDescription:
//    case LookupType::TypeAlbumImage:
//    case LookupType::TypeAlbumDate:
//    case LookupType::TypeArtistDescription:
//    case LookupType::TypeArtistImage:
//    case LookupType::TypeTrackDescription:
//    case LookupType::TypeUndefined:
//    case LookupType::TypeLyrics:
//        return true;
//    default:
//        return false;
//    }
//}

//http://ws.audioscrobbler.com/2.0/?method=album.search&album=F.I.R&api_key=625fd47b3b685af19315cc3a1aa5920a&format=json
//void LastFmLookup::lookup(const AudioMetaObject &object)
//{
//    qDebug()<<"====> "<<Q_FUNC_INFO<<" <====";

//#if 0
//    emit lookupFailed ();
//    return;
//#endif

//    QUrl url(API_BASE);
//    QUrlQuery query;
//    query.addQueryItem ("api_key", QString(API_KEY));
//    query.addQueryItem ("format", QString("json"));

//    switch (currentLookupFlag ()) {
//    case LookupType::TypeAlbumImage:
//    case LookupType::TypeAlbumDate:
//    case LookupType::TypeAlbumDescription: {
//        QString artist = formatStr (/*(*object)->artistMeta ()->name ()*/object.artistMeta ().name ());
//        QString album = formatStr (/*(*object)->albumMeta ()->name ()*/object.albumMeta ().name ());
//        if (!artist.isEmpty () && !album.isEmpty ()) {
//            query.addQueryItem ("artist", artist);
//            query.addQueryItem ("album", album);
//            query.addQueryItem ("method", QString("album.getInfo"));
//        } else {
//            //TODO: last.fm alum info need artist name
//            emit lookupFailed ();
//            return;
//        }
//        break;
//    }
//    case LookupType::TypeArtistImage:
//    case LookupType::TypeArtistDescription: {
//        QString artist = formatStr (/*(*object)->artistMeta ()->name ()*/object.artistMeta ().name ());
//        if (!artist.isEmpty ()) {
//            query.addQueryItem ("artist", artist);
//            query.addQueryItem ("method", QString("artist.getInfo"));
//        } else {
//            emit lookupFailed ();
//            return;
//        }
//        break;
//    }
//    case LookupType::TypeTrackDescription: {
//        QString artist = formatStr (/*(*object)->artistMeta ()->name ()*/object.artistMeta ().name ());
//        QString track = formatStr (/*(*object)->trackMeta ()->title ()*/object.trackMeta ().title ());
//        if (track.isEmpty ()) {
//            track = object.name ();
//            //TODO: quick hack
//            track = track.mid (0, track.indexOf ("."));
//        }
//        if (!artist.isEmpty () && !track.isEmpty ()) {
//            query.addQueryItem ("artist", artist);
//            query.addQueryItem ("track", track);
//            query.addQueryItem ("method", QString("track.getInfo"));
//        } else {
//            //TODO: last.fm track info need artist name
//            emit lookupFailed ();
//            return;
//        }
//        break;
//    }
//    default:
//        emit lookupFailed ();
//        return;
//    }
//    url.setQuery (query);

//    qDebug()<<Q_FUNC_INFO<<" URL "<<url <<" network is null "<<(m_networkLookup == nullptr);

//    m_networkLookup->setUrl (url.toString ());
//    m_networkLookup->setRequestType (BaseNetworkLookup::RequestGet);
//    m_networkLookup->startLookup ();
//}

QByteArray LastFmLookup::parseSongDescription(const QByteArray &qba) const
{
    QJsonObject mainObj;
    if (!parseRootObject(mainObj, qba, QString("track"))) {
        return QByteArray();
    }
    if (mainObj.isEmpty()) {
        qDebug()<<"mainObj is isEmpty";
        return QByteArray();
    }
    QJsonValue v = mainObj.value(QString("wiki"));
    if (v.isUndefined()) {
        qDebug()<<"wiki is isEmpty";
        return QByteArray();
    }
    QJsonObject o = v.toObject();
    if (o.isEmpty()) {
        qDebug()<<"wiki obj is isEmpty";
        return QByteArray();
    }
    v = o.value(QString("content"));
    if (v.isUndefined ()) {
        qDebug()<<"wiki content is isUndefined";
        return QByteArray();
    }
    return v.toString().toUtf8();
}

QByteArray LastFmLookup::parseAlbumData(const QByteArray &qba, SupportedType type) const
{
    QJsonObject mainObj;
    if (!parseRootObject(mainObj, qba, QString("album"))) {
        return QByteArray();
    }
    if (mainObj.isEmpty()) {
        qDebug()<<"mainObj is isEmpty";
        return QByteArray();
    }
    if (type == SupportLookupAlbumDate) {
        QJsonValue v = mainObj.value(QString("releasedate"));
        if (v.isUndefined()) {
            qDebug()<<"date is isEmpty";
            return QByteArray();
        }
        return v.toString().toUtf8();
    }
    if (type == SupportLookupAlbumDescription) {
        QJsonValue v = mainObj.value(QString("wiki"));
        if (v.isUndefined()) {
            qDebug()<<"wiki is isEmpty";
            return QByteArray();
        }
        QJsonObject o = v.toObject();
        if (o.isEmpty()) {
            qDebug()<<"wiki obj is isEmpty";
            return QByteArray();
        }
        v = o.value(QString("content"));
        if (v.isUndefined ()) {
            qDebug()<<"wiki content is isUndefined";
            return QByteArray();
        }
        return v.toString().toUtf8();
    }
    if (type == SupportLookupAlbumImage) {
        QJsonValue v = mainObj.value(QString("image"));
        if (v.isUndefined() || !v.isArray()) {
            qDebug()<<"image is isEmpty or no image array found";
            return QByteArray();
        }
        QJsonArray array = v.toArray();
        QString str;
        foreach (const QJsonValue &value, array) {
            if (value.isUndefined()) {
                continue;
            }
            QJsonObject o = value.toObject();
            if (o.isEmpty()) {
                continue;
            }
            if (o.value(QString("size")).toString() == m_imageSizeValue) {
                str = o.value(QString("#text")).toString();
                break;
            }
        }
        return str.toUtf8();
    }
    return QByteArray();
}

QByteArray LastFmLookup::parseArtisData(const QByteArray &qba, SupportedType type) const
{
    QJsonObject mainObj;
    if (!parseRootObject(mainObj, qba, QString("artist"))) {
        return QByteArray();
    }
    if (mainObj.isEmpty()) {
        qDebug()<<"mainObj is isEmpty";
        return QByteArray();
    }
    if (type == SupportLookupArtistDescription) {
        QJsonValue v = mainObj.value(QString("bio"));
        if (v.isUndefined()) {
            qDebug()<<"bio is isUndefined";
            return QByteArray();
        }
        QJsonObject o = v.toObject();
        if (o.isEmpty ()) {
            qDebug()<<"bio obj is isEmpty";
            return QByteArray();
        }
        v = o.value(QString("content"));
        if (v.isUndefined()) {
            qDebug()<<"content is isUndefined";
            return QByteArray();
        }
        return v.toString().toUtf8();
    }
    if (type == SupportLookupArtistImage) {
        QJsonValue v = mainObj.value(QString("image"));
        if (v.isUndefined() || !v.isArray()) {
            qDebug()<<"image is isEmpty or no image array found";
            return QByteArray();
        }
        QJsonArray array = v.toArray();
        QString str;
        foreach (const QJsonValue &value, array) {
            if (value.isUndefined()) {
                continue;
            }
            QJsonObject o = value.toObject();
            if (o.isEmpty()) {
                continue;
            }
            if (o.value(QString("size")).toString() == m_imageSizeValue) {
                str = o.value(QString("#text")).toString();
                break;
            }
        }
        return str.toUtf8();
    }
    return QByteArray();
}

bool LastFmLookup::parseRootObject(QJsonObject &out, const QByteArray &in, const QString &key) const
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(in, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug()<<error.errorString ();
        return false;
    }
    if (!doc.isObject()) {
        qDebug()<<"data is not json obj";
        return false;
    }
    QJsonObject docObj = doc.object();
    QJsonValue mainValue = docObj.value(key);
    if (mainValue.isUndefined()) {
        qDebug()<<"mainValue  for key "<<key<<" is undefined";
        return false;
    }
    out = mainValue.toObject();
    return true;
}

QString LastFmLookup::formatStr(const QString &in) const
{
    QString s = in;
    s = s.replace('#',"%23");
    s = s.replace('&',"%26");
    s = s.replace('+',"%2B");
    s = s.replace(' ',"+");
    return s;
}

void LastFmLookup::setErrorMsg(QString *msgPtr, const QString &msgData) const
{
    if (msgPtr) {
        msgPtr->append(msgData);
    }
}

void LastFmLookup::setRequestHeader(QCurl::QCNetworkRequest *req) const
{
    if (req) {
        req->setRawHeader("User-Agent",
                          //"Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-CN;rv:1.9.2.9) Gecko/20100101 Firefox/43.0"
                          "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:70.0) Gecko/20100101 Firefox/70.0");
        req->setRawHeader("Accept",
                          "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");

    }
}

QByteArray LastFmLookup::networkCall(const QCNetworkRequest &request, QString *errorMsg) const
{
    QCNetworkSyncReply *reply = m_network->create(request);
    if (!reply) {
        if (errorMsg) {
            errorMsg->append("Cant't create network reply!");
        }
        return QByteArray();
    }
    bool requestAborted = false;
    QByteArray replyData;

    reply->setWriteFunction([&](char *buffer, size_t size) ->size_t {
        replyData.append(buffer, static_cast<int>(size));
        return size;
    });
    reply->setCustomHeaderFunction([&](char *buffer, size_t size)->size_t {
        if (!replyData.isEmpty()) {
            const QByteArray header(buffer, static_cast<int>(size));
            const int pos = replyData.indexOf(header);
            if (pos >= 0) {
                replyData.remove(pos, static_cast<int>(size));
            }
        }
        return size;
    });

    m_timer->setInterval(TIMER_INTERVAL_MS);
    connect(m_timer, &QTimer::timeout, this, [&]() {
        requestAborted = true;
        reply->abort();
        reply->deleteLater();
        reply = Q_NULLPTR;
        if (errorMsg) {
            errorMsg->append("Request timeout, aborted!");
        }
    });
    m_timer->start();

    //block event and run network request
    reply->perform();

    //finish network request

    if (m_timer->isActive()) {
        m_timer->stop();
    }
    m_timer->disconnect();

    if (requestAborted) {
        if (reply) {
            reply->abort();
            reply->deleteLater();
            reply = Q_NULLPTR;
        }
        return QByteArray();
    }
    const NetworkError error = reply->error();
    const QString errorStr = reply->errorString();
    if (error != NetworkNoError) {
        if (errorMsg) {
            errorMsg->append(errorStr);
        }
        if (reply) {
            reply->deleteLater();
            reply = Q_NULLPTR;
        }
        return QByteArray();
    }
    if (reply) {
        reply->abort();
        reply->deleteLater();
        reply = Q_NULLPTR;
    }
    return replyData;
}

} //LastFmLookup
} //DataProvider
} //PhoenixPlayer
