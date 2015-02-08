#ifndef BAIDULOOKUP_H
#define BAIDULOOKUP_H

#include <QObject>

#include "MetadataLookup/IMetadataLookup.h"

class QTextCodec;
namespace PhoenixPlayer{
class SongMetaData;
namespace MetadataLookup {
class BaseNetworkLookup;
namespace BaiduLookup {

#define PLUGIN_NAME "BaiduLyricsLookup"
#define PLUGIN_VERSION "0.1"
class BaiduLookup : public IMetadataLookup
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.MetadataLookup.BaiduLyricsLookup" FILE "lyricsbaidu.json")
    Q_INTERFACES(PhoenixPlayer::MetadataLookup::IMetadataLookup)
public:
    BaiduLookup(QObject *parent = 0);
    virtual ~BaiduLookup();

public:
    QString getUrl();

     // ILyricsLookup interface
public:
    QString getPluginName();
    QString getPluginVersion();
    void lookup(SongMetaData *meta);
    bool supportLookup(LookupType type);

private:
    QTextCodec *mGBKCodec;
    SongMetaData *mMeta;
    BaseNetworkLookup *mLrcidDL;
    BaseNetworkLookup *mLyricsDL;
};

} //BaiduLookup
} //Lyrics
} //PhoenixPlayer
#endif // BAIDULOOKUP_H
