
#include <QRegularExpression>
#include <QTextCodec>
#include <QUrlQuery>
#include <QDebug>

#include "Lyrics/ILyricsLookup.h"
#include "BaiduLookup.h"
#include "SongMetaData.h"
#include "Common.h"
#include "BaseNetworkLookup.h"

namespace PhoenixPlayer{
class SongMetaData;
namespace Lyrics {
namespace BaiduLookup {

BaiduLookup::BaiduLookup(QObject *parent)
    : ILyricsLookup(parent)/*, BaseNetworkLookup(parent)*/
{
     mGBKCodec = QTextCodec::codecForName("GBK");
     mMeta = new SongMetaData(this);
     mLrcidDL = new BaseNetworkLookup(this);
     mLyricsDL = new BaseNetworkLookup(this);

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

     connect (mLyricsDL,
              &BaseNetworkLookup::failed,
              [this](const QUrl &requestedUrl, const QString &error) {
         Q_UNUSED(requestedUrl);
         qDebug()<<"BaiduLookup download lyric failed "<<error;
         emit lookupFailed ();
     });

     connect (mLyricsDL,
              &BaseNetworkLookup::succeed,
              [this](const QUrl &requestedUrl, const QByteArray &replyData) {
         Q_UNUSED(requestedUrl)
         if (replyData.isEmpty ()) {
             qDebug()<<"BaiduLookup download empty lyric";
             emit lookupFailed ();
             return;
         }
         emit lookupSucceed (mGBKCodec->toUnicode (replyData));
     });
}

BaiduLookup::~BaiduLookup()
{
    if (mMeta != nullptr)
        mMeta->deleteLater ();

    if (mLrcidDL != nullptr)
        mLrcidDL->deleteLater ();

    if (mLyricsDL != nullptr)
        mLyricsDL->deleteLater ();
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

QString BaiduLookup::getPluginVersion()
{
    return QString(PLUGIN_VERSION);
}

void BaiduLookup::lookup(SongMetaData *meta)
{
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

} //BaiduLookup
} //Lyrics
} //PhoenixPlayer
