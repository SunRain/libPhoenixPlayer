#ifndef LASTFMLOOKUP_H
#define LASTFMLOOKUP_H

#include <QObject>
#include <MetadataLookup/IMetadataLookup.h>

class QByteArray;
class QJsonObject;
namespace PhoenixPlayer{
class SongMetaData;

namespace MetadataLookup {
class BaseNetworkLookup;

namespace LastFmLookup {

#define LASTFMAPIKEY "48570d887cca45f05f8094dca4f79c0d"
#define API_BASE "http://ws.audioscrobbler.com/2.0"
#define API_KEY "625fd47b3b685af19315cc3a1aa5920a"

/*
 *  macro based on last.fm json result
 *  should use a configuration later
 *
 */
#define IMAGE_SIZE_REGEXP_LARGE "large"
#define IMAGE_SIZE_REGEXP_MEDIA "medium"
#define IMAGE_SIZE_REGEXP_SMALL "small"
#define IMAGE_SIZE_REGEXP_EXTRA_LARGE "extralarge"
#define IMAGE_SIZE_REGEXP_MEGA "mega"

#define CONFIG_KEY "LastFM_Image_Size"

#define DEFAULT_IMAGE_SIZE_REGEXP IMAGE_SIZE_REGEXP_MEDIA

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
    bool supportLookup(LookupType type);
    void lookup(SongMetaData *meta);

protected:
//    void parseAlbumDescription(const QByteArray &qba);
//    void parseAlbumImage(const QByteArray &qba);
//    void parseArtistDescription(const QByteArray &qba);
//    void parseArtistImage(const QByteArray &qba);
    void parseSongDescription(const QByteArray &qba);
    void parseAlbumData(const QByteArray &qba);
    void parseArtisData(const QByteArray &qba);

private:
    bool parseRootObject(QJsonObject &out, const QByteArray &in, const QString &key);
    QString formatStr(const QString &in);
private:
    BaseNetworkLookup *mNetworkLookup;
    QString mImageSizeValue;
};

} //LastFmLookup
} //MetadataLookup
} //PhoenixPlayer
#endif // LASTFMLOOKUP_H
