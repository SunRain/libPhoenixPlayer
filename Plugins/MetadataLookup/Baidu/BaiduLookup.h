#ifndef BAIDULOOKUP_H
#define BAIDULOOKUP_H

#include <QObject>

#include "MetadataLookup/IMetadataLookup.h"

class QTextCodec;
class QByteArray;
namespace PhoenixPlayer{
class SongMetaData;
namespace MetadataLookup {
class BaseNetworkLookup;
namespace BaiduLookup {

class BaiduLookup : public IMetadataLookup
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.MetadataLookup.BaiduLyricsLookup" FILE "lyricsbaidu.json")
    Q_INTERFACES(PhoenixPlayer::MetadataLookup::IMetadataLookup)
public:
    BaiduLookup(QObject *parent = 0);
    virtual ~BaiduLookup();

     // ILyricsLookup interface
public:
    void lookup(SongMetaData *meta);
    bool supportLookup(LookupType type);
private slots:
    void dlFailed(const QUrl &requestedUrl, const QString &error);
    void dlSucceed(const QUrl &requestedUrl, const QByteArray &replyData);
    QString formatStr(const QString &in);
private:
    QTextCodec *mGBKCodec;
    BaseNetworkLookup *mNDL;
    bool mIsLrcidDL;
};

} //BaiduLookup
} //Lyrics
} //PhoenixPlayer
#endif // BAIDULOOKUP_H
