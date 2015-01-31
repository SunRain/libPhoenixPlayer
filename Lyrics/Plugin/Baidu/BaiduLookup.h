#ifndef BAIDULOOKUP_H
#define BAIDULOOKUP_H

#include <QObject>

#include "Lyrics/ILyricsLookup.h"

class QTextCodec;
namespace PhoenixPlayer{
class SongMetaData;
namespace Lyrics {
class BaseNetworkLookup;
namespace BaiduLookup {

#define PLUGIN_NAME "BaiduLyricsLookup"
#define PLUGIN_VERSION "0.1"
class BaiduLookup : public ILyricsLookup/*, BaseNetworkLookup*/
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.Lyrics.BaiduLookup" FILE "lyricsbaidu.json")
    Q_INTERFACES(PhoenixPlayer::Lyrics::ILyricsLookup)
public:
    BaiduLookup(QObject *parent = 0);
    ~BaiduLookup();

public:
    QString getUrl();

     // ILyricsLookup interface
public:
    QString getPluginName();
    QString getPluginVersion();
    void lookup(SongMetaData *meta);


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
