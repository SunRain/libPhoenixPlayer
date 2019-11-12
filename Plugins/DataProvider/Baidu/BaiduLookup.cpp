
#include <QRegularExpression>
#include <QTextCodec>
#include <QUrlQuery>
#include <QDebug>
#include <QTimer>

#include "BaiduLookup.h"
#include "AudioMetaObject.h"
#include "PPCommon.h"
#include "MetadataLookup/BaseNetworkLookup.h"

#include "QCNetworkAccessManager.h"
#include "QCNetworkSyncReply.h"

namespace PhoenixPlayer{
namespace DataProvider {
namespace BaiduLookup {

using namespace QCurl;

const static int TIMER_INTERVAL_MS = 10000; //10s

BaiduLookup::BaiduLookup(QObject *parent)
    : IMetadataLookup(parent)
{
     m_GBKCodec = QTextCodec::codecForName("GBK");
//     m_NDL = new BaseNetworkLookup(this);
//     m_isLrcidDL = true;

//     connect (m_NDL, &BaseNetworkLookup::failed, this, &BaiduLookup::dlFailed);
//     connect (m_NDL, &BaseNetworkLookup::succeed, this, &BaiduLookup::dlSucceed);
     m_network = new QCNetworkAccessManager(this);

     m_timer = new QTimer(this);
     m_timer->setSingleShot(true);
//     connect(m_timer, &QTimer::timeout, this, [&]() {
//         m_requestAborted = true;
//         if (m_reply) {
//             m_reply->abort();
//             m_reply->deleteLater();
//             m_reply = Q_NULLPTR;
//         }
//     });
}

BaiduLookup::~BaiduLookup()
{
    qDebug()<<Q_FUNC_INFO;
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
        m_timer->deleteLater();
        m_timer = Q_NULLPTR;
    }
//    if (m_reply) {
//        m_reply->abort();
//        m_reply->deleteLater();
//        m_reply = Q_NULLPTR;
//    }
    if (m_network) {
        m_network->deleteLater();
        m_network = Q_NULLPTR;
    }
}

PluginProperty BaiduLookup::property() const
{
    return PluginProperty("BaiduLyricsLookup",
                          "1.0",
                          "Lyrics lookup from baidu.com",
                          false,
                          false);
}

bool BaiduLookup::support(IDataProvider::SupportedTypes type) const
{
    if ((type & SupportLookupLyrics) == SupportLookupLyrics) {
        return true;
    }
    return false;
}

QByteArray BaiduLookup::lookup(const AudioMetaObject &object, IDataProvider::SupportedType type, QString *errorMsg) const
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

    QByteArray replyData;

    if (m_timer->isActive()) {
        m_timer->disconnect();
        m_timer->stop();
    }

    // setp one
    {
        QString name = object.trackMeta().title();
        if (name.isEmpty()) {
            name = object.name();
            //TODO: quick hack
            name = name.mid(0, name.indexOf("."));
        }
        QString artist = object.artistMeta().name();

        QUrl url("http://box.zhangmen.baidu.com/x");
        QUrlQuery query;
        query.addQueryItem("title", QString("%1$$%2$$$$").arg(formatStr(name)).arg(formatStr(artist)));
        query.addQueryItem("op", "12");
        query.addQueryItem("count", "1");
        query.addQueryItem("format", "json");
        url.setQuery(query);

        QCNetworkRequest request(url);
        this->setRequestHeader(&request);

        replyData = networkCall(request, errorMsg);

        if (replyData.isEmpty()) {
            if (errorMsg) {
                errorMsg->append("Empty reply data!");
            }
            return QByteArray();
        }
    }

    // step two
    {
        QRegularExpression lyricsID("<lrcid>([0-9]*)</lrcid>");
        QRegularExpressionMatchIterator i = lyricsID.globalMatch(QString(replyData));
        if (!i.hasNext()) {
            if (errorMsg) {
                errorMsg->append("BaiduLookup No lyricsID match found !");
            }
            return QByteArray();
        }

        QString currentID = i.next().captured(1);
        QString str = QString("http://box.zhangmen.baidu.com/bdlrc/%1/%2.lrc")
                          .arg(QString::number(currentID.toLongLong()/100))
                          .arg(currentID);
        QCNetworkRequest request(str);
        setRequestHeader(&request);

        replyData.clear();
        replyData = networkCall(request, errorMsg);
        if (replyData.isEmpty()) {
            if (errorMsg) {
                errorMsg->append("Can't find lyrics reply data!");
            }
            return QByteArray();
        }
        return m_GBKCodec->toUnicode(replyData).toUtf8();

    }
}

//void BaiduLookup::lookup(const AudioMetaObject &object)
//{
//    if (object.isEmpty ()) {
//        qDebug()<<Q_FUNC_INFO<<"[BaiduLookup] No meta found";
//        emit lookupFailed();
//        return;
//    }
//    m_isLrcidDL = true;

//    QString name = object.trackMeta ().title ();//(*object)->trackMeta ()->title ();
//    if (name.isEmpty ()) {
//        name = object.name ();//(*object)->name ();
//        //TODO: quick hack
//        name = name.mid (0, name.indexOf ("."));
//    }
//    QString artist = object.artistMeta ().name ();//(*object)->artistMeta ()->name ();

//    QUrl url("http://box.zhangmen.baidu.com/x");
//    QUrlQuery query;
//    query.addQueryItem("title", QString("%1$$%2$$$$").arg (formatStr (name)).arg (formatStr (artist)));
//    query.addQueryItem("op", "12");
//    query.addQueryItem("count", "1");
//    query.addQueryItem("format", "json");
//    url.setQuery(query);





//    m_NDL->setUrl (url.toString ());
//    m_NDL->setRequestType (BaseNetworkLookup::RequestType::RequestGet);
//    m_NDL->startLookup ();
//}

//bool BaiduLookup::supportLookup(IMetadataLookup::LookupType type)
//{
//    switch (type) {
//    case LookupType::TypeUndefined:
//    case LookupType::TypeLyrics:
//        return true;
//    case LookupType::TypeAlbumDescription:
//    case LookupType::TypeAlbumImage:
//    case LookupType::TypeAlbumDate:
//    case LookupType::TypeArtistDescription:
//    case LookupType::TypeArtistImage:
//    case LookupType::TypeTrackDescription:
//    default:
//        return false;
//    }
//}

//void BaiduLookup::dlFailed(const QUrl &requestedUrl, const QString &error)
//{
//    qDebug()<<Q_FUNC_INFO<<QString("Failed for url [%1] with error [%2]")
//              .arg (requestedUrl.toString ()).arg (error);
//    emit lookupFailed ();
//}

//void BaiduLookup::dlSucceed(const QUrl &requestedUrl, const QByteArray &replyData)
//{
//    qDebug()<<Q_FUNC_INFO<<QString("Succeed for url [%1] with data [%2]")
//              .arg (requestedUrl.toString ()).arg (QString(replyData));

//    if (m_isLrcidDL) {
//        qDebug()<<Q_FUNC_INFO<<"Current is for lyrics id download";

//        QRegularExpression lyricsID("<lrcid>([0-9]*)</lrcid>");
//        QRegularExpressionMatchIterator i = lyricsID.globalMatch(QString(replyData));
//        if(!i.hasNext()) {
//            qDebug()<<Q_FUNC_INFO<<"BaiduLookup No match found";
//            emit lookupFailed ();
//            return;
//        }
//        m_isLrcidDL = false;
//        //Use the first id.
//        QString currentID = i.next().captured(1);
//        QString str = QString("http://box.zhangmen.baidu.com/bdlrc/%1/%2.lrc")
//                .arg (QString::number(currentID.toLongLong()/100))
//                .arg (currentID);

//        m_NDL->setUrl (str);
//        m_NDL->setRequestType (BaseNetworkLookup::RequestType::RequestGet);
//        m_NDL->startLookup ();
//    } else {
//        if (replyData.isEmpty ()) {
//            qDebug()<<Q_FUNC_INFO<<"BaiduLookup download empty lyric";
//            emit lookupFailed ();
//            return;
//        }
//        emit lookupSucceed (m_GBKCodec->toUnicode (replyData).toLocal8Bit ());
//    }
//}

QString BaiduLookup::formatStr(const QString &in) const
{
    QString s = in;
    s = s.replace('#',"%23");
    s = s.replace('&',"%26");
    s = s.replace('+',"%2B");
    s = s.replace(' ',"+");
    return s;
}

void BaiduLookup::setRequestHeader(QCNetworkRequest *req) const
{
    if (req) {
        req->setRawHeader("User-Agent",
                          //"Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-CN;rv:1.9.2.9) Gecko/20100101 Firefox/43.0"
                          "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:70.0) Gecko/20100101 Firefox/70.0");
        req->setRawHeader("Accept",
                          "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");

    }
}

QByteArray BaiduLookup::networkCall(const QCNetworkRequest &request, QString *errorMsg) const
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



} //BaiduLookup
} //DataProvider
} //PhoenixPlayer
