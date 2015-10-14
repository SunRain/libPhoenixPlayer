
#include <QRegularExpression>
#include <QTextCodec>
#include <QUrlQuery>
#include <QDebug>

#include "BaiduLookup.h"
#include "SongMetaData.h"
#include "Common.h"
#include "MetadataLookup/BaseNetworkLookup.h"

namespace PhoenixPlayer{
namespace MetadataLookup {
namespace BaiduLookup {

BaiduLookup::BaiduLookup(QObject *parent)
    : IMetadataLookup(parent)
{
     m_GBKCodec = QTextCodec::codecForName("GBK");
     m_NDL = new BaseNetworkLookup(this);
     m_isLrcidDL = true;

     connect (m_NDL, &BaseNetworkLookup::failed, this, &BaiduLookup::dlFailed);
     connect (m_NDL, &BaseNetworkLookup::succeed, this, &BaiduLookup::dlSucceed);
}

BaiduLookup::~BaiduLookup()
{
    qDebug()<<Q_FUNC_INFO;
}

void BaiduLookup::lookup(SongMetaData **meta)
{
    if (!meta) {
        qDebug()<<Q_FUNC_INFO<<"[BaiduLookup] No meta found";
        emit lookupFailed();
        return;
    }
    m_isLrcidDL = true;

    QString name = (*meta)->trackMeta ()->title ();
    if (name.isEmpty ()) {
        name = (*meta)->name ();
        //TODO: quick hack
        name = name.mid (0, name.indexOf ("."));
    }
    QString artist = (*meta)->artistMeta ()->name ();

    QUrl url("http://box.zhangmen.baidu.com/x");
    QUrlQuery query;
    query.addQueryItem ("title", QString("%1$$%2$$$$").arg (formatStr (name)).arg (formatStr (artist)));
    query.addQueryItem ("op", "12");
    query.addQueryItem ("count", "1");
    query.addQueryItem ("format", "json");
    url.setQuery (query);

    m_NDL->setUrl (url.toString ());
    m_NDL->setRequestType (BaseNetworkLookup::RequestType::RequestGet);
    m_NDL->startLookup ();
}

bool BaiduLookup::supportLookup(IMetadataLookup::LookupType type)
{
    switch (type) {
    case LookupType::TypeUndefined:
    case LookupType::TypeLyrics:
        return true;
    case LookupType::TypeAlbumDescription:
    case LookupType::TypeAlbumImage:
    case LookupType::TypeAlbumDate:
    case LookupType::TypeArtistDescription:
    case LookupType::TypeArtistImage:
    case LookupType::TypeTrackDescription:
    default:
        return false;
    }
}

void BaiduLookup::dlFailed(const QUrl &requestedUrl, const QString &error)
{
    qDebug()<<Q_FUNC_INFO<<QString("Failed for url [%1] with error [%2]")
              .arg (requestedUrl.toString ()).arg (error);
    emit lookupFailed ();
}

void BaiduLookup::dlSucceed(const QUrl &requestedUrl, const QByteArray &replyData)
{
    qDebug()<<Q_FUNC_INFO<<QString("Succeed for url [%1] with data [%2]")
              .arg (requestedUrl.toString ()).arg (QString(replyData));

    if (m_isLrcidDL) {
        qDebug()<<Q_FUNC_INFO<<"Current is for lyrics id download";

        QRegularExpression lyricsID("<lrcid>([0-9]*)</lrcid>");
        QRegularExpressionMatchIterator i = lyricsID.globalMatch(QString(replyData));
        if(!i.hasNext()) {
            qDebug()<<Q_FUNC_INFO<<"BaiduLookup No match found";
            emit lookupFailed ();
            return;
        }
        m_isLrcidDL = false;
        //Use the first id.
        QString currentID = i.next().captured(1);
        QString str = QString("http://box.zhangmen.baidu.com/bdlrc/%1/%2.lrc")
                .arg (QString::number(currentID.toLongLong()/100))
                .arg (currentID);

        m_NDL->setUrl (str);
        m_NDL->setRequestType (BaseNetworkLookup::RequestType::RequestGet);
        m_NDL->startLookup ();
    } else {
        if (replyData.isEmpty ()) {
            qDebug()<<Q_FUNC_INFO<<"BaiduLookup download empty lyric";
            emit lookupFailed ();
            return;
        }
        emit lookupSucceed (m_GBKCodec->toUnicode (replyData).toLocal8Bit ());
    }
}

QString BaiduLookup::formatStr(const QString &in)
{
    QString s = in;
    s = s.replace('#',"%23");
    s = s.replace('&',"%26");
    s = s.replace('+',"%2B");
    s = s.replace(' ',"+");
    return s;
}
} //BaiduLookup
} //Lyrics
} //PhoenixPlayer
