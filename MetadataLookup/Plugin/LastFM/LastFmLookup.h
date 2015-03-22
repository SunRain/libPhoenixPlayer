#ifndef LASTFMLOOKUP_H
#define LASTFMLOOKUP_H

#include <QObject>
#include <MetadataLookup/IMetadataLookup.h>

class QByteArray;
namespace PhoenixPlayer{
class SongMetaData;

namespace MetadataLookup {
class BaseNetworkLookup;

namespace LastFmLookup {

#define PLUGIN_NAME "LastFmMetadataLookup"
#define PLUGIN_VERSION "0.1"
#define DESCRIPTION "Lookup track metadata from last.fm"
#define LASTFMAPIKEY "48570d887cca45f05f8094dca4f79c0d"

#define API_BASE "http://ws.audioscrobbler.com/2.0"
#define API_KEY "625fd47b3b685af19315cc3a1aa5920a"

class LastFmLookup : public IMetadataLookup
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.MetadataLookup.LastFmLookup" FILE "lastfmlookup.json")
    Q_INTERFACES(PhoenixPlayer::MetadataLookup::IMetadataLookup)
public:
    LastFmLookup(QObject *parent = 0);
    virtual ~LastFmLookup();

    // IMetadataLookup interface
public:
    QString getPluginName();
    QString getPluginVersion();
    QString getDescription();
    bool supportLookup(LookupType type);
    void lookup(SongMetaData *meta);

protected:
    void parseAlbumDescription(const QByteArray &qba);
    void parseAlbumImage(const QByteArray &qba);
    void parseArtistDescription(const QByteArray &qba);
    void parseArtistImage(const QByteArray &qba);
    void parseSongDescription(const QByteArray &qba);

private:
//    void lookupByFlag(const LookupType &type);
//    void parseResult(const QByteArray &qba);
private:
    BaseNetworkLookup *mNetworkLookup;
//    SongMetaData *mSongMeta;
};

} //LastFmLookup
} //MetadataLookup
} //PhoenixPlayer
#endif // LASTFMLOOKUP_H
