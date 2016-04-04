#ifndef BAIDULOOKUP_H
#define BAIDULOOKUP_H

#include <QObject>

#include "MetadataLookup/IMetadataLookup.h"

class QTextCodec;
class QByteArray;
namespace PhoenixPlayer{
class AudioMetaObject;
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
    void lookup(AudioMetaObject **meta);
    bool supportLookup(LookupType type);
private slots:
    void dlFailed(const QUrl &requestedUrl, const QString &error);
    void dlSucceed(const QUrl &requestedUrl, const QByteArray &replyData);
    QString formatStr(const QString &in);
private:
    QTextCodec *m_GBKCodec;
    BaseNetworkLookup *m_NDL;
    bool m_isLrcidDL;
};

} //BaiduLookup
} //Lyrics
} //PhoenixPlayer
#endif // BAIDULOOKUP_H
