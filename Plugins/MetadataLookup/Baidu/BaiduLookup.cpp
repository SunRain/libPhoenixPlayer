
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
    : IMetadataLookup(parent)/*, BaseNetworkLookup(parent)*/
{
     mGBKCodec = QTextCodec::codecForName("GBK");
     mMeta = new SongMetaData(this);
     mLrcidDL = new BaseNetworkLookup(this);
     mLyricsDL = new BaseNetworkLookup(this);

     this->setCurrentLookupFlag (IMetadataLookup::LookupType::TypeLyrics);

     //搜索lrcid错误
     connect (mLrcidDL,
              &BaseNetworkLookup::failed,
              [this](const QUrl &requestedUrl, const QString &error) {
         Q_UNUSED(requestedUrl);
         qDebug()<<"BaiduLookup failed "<<error;
         emit lookupFailed ();
     });

     //搜索lycid成功
     connect (mLrcidDL,
              &BaseNetworkLookup::succeed,
              [this](const QUrl &requestedUrl, const QByteArray &replyData) {
         Q_UNUSED(requestedUrl)
         qDebug()<<"BaiduLookup download lyric id";

         QRegularExpression lyricsID("<lrcid>([0-9]*)</lrcid>");
         QRegularExpressionMatchIterator i = lyricsID.globalMatch(QString(replyData));
         if(!i.hasNext()) {
             qDebug()<<"BaiduLookup No match found";
             emit lookupFailed ();
             return;
         }
         //Use the first id.
         QString currentID = i.next().captured(1);
         QString str = QString("http://box.zhangmen.baidu.com/bdlrc/%1/%2.lrc")
                 .arg (QString::number(currentID.toLongLong()/100))
                 .arg (currentID);

         //Get the lyrics!
         mLyricsDL->setUrl (str);
         mLyricsDL->setRequestType (BaseNetworkLookup::RequestType::RequestGet);
         mLyricsDL->startLookup ();
     });

     //下载lyrics失败
     connect (mLyricsDL,
              &BaseNetworkLookup::failed,
              [this](const QUrl &requestedUrl, const QString &error) {
         Q_UNUSED(requestedUrl);
         qDebug()<<"BaiduLookup download lyric failed "<<error;
         emit lookupFailed ();
     });

     //下载lyrics成功
     connect (mLyricsDL,
              &BaseNetworkLookup::succeed,
              [this](const QUrl &requestedUrl, const QByteArray &replyData) {
         Q_UNUSED(requestedUrl)
         if (replyData.isEmpty ()) {
             qDebug()<<"BaiduLookup download empty lyric";
             emit lookupFailed ();
             return;
         }
         emit lookupSucceed (mGBKCodec->toUnicode (replyData).toUtf8 ());
     });
}

BaiduLookup::~BaiduLookup()
{
    qDebug()<<__FUNCTION__;
//    if (mMeta != nullptr)
//        mMeta->deleteLater ();

//    if (mLrcidDL != nullptr) {
//        mLrcidDL->deleteLater ();
//    }

//    if (mLyricsDL != nullptr)
//        mLyricsDL->deleteLater ();

//    qDebug()<<"after"<<__FUNCTION__;
}

QString BaiduLookup::getUrl()
{
   QString name = mMeta->getMeta (Common::SongMetaTags::E_SongTitle).toString ();
   if (name.isEmpty ()) {
       name = mMeta->getMeta (Common::SongMetaTags::E_FileName).toString ();
       //TODO: quick hack
       name = name.mid (0, name.indexOf ("."));
   }
   if (name.isEmpty ())
       return QString();

   QString artist = mMeta->getMeta (Common::SongMetaTags::E_ArtistName)
           .toString ();

   QUrl url("http://box.zhangmen.baidu.com/x");
   QUrlQuery query;
   query.addQueryItem ("title", QString("%1$$%2$$$$").arg (name).arg (artist));
   query.addQueryItem ("op", "12");
   query.addQueryItem ("count", "1");
   query.addQueryItem ("format", "json");
   url.setQuery (query);
   return url.toString ();
}

QString BaiduLookup::getPluginName()
{
    return QString(PLUGIN_NAME);
}

void BaiduLookup::lookup(SongMetaData *meta)
{
    if (meta == nullptr) {
        qDebug()<<"[BaiduLookup] No meta found";
        return;
    }
    for (int i = (int)(Common::SongMetaTags::E_FirstFlag) +1;
         i < (int)Common::SongMetaTags::E_LastFlag;
         ++i) {
        mMeta->setMeta (Common::SongMetaTags(i),
                        meta->getMeta (Common::SongMetaTags(i)));
    }

    mLrcidDL->setUrl (getUrl ());
    mLrcidDL->setRequestType (BaseNetworkLookup::RequestType::RequestGet);
    mLrcidDL->startLookup ();
}

bool BaiduLookup::supportLookup(IMetadataLookup::LookupType type)
{
    switch (type) {
    case LookupType::TypeUndefined:
        return true;
    case LookupType::TypeLyrics:
        return true;
    default:
        return false;
    }
}

} //BaiduLookup
} //Lyrics
} //PhoenixPlayer
