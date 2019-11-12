#ifndef LASTFMLOOKUP_H
#define LASTFMLOOKUP_H

#include <QObject>
#include <QByteArray>

#include "libphoenixplayer_global.h"
#include "DataProvider/IMetadataLookup.h"

class QTimer;

namespace QCurl {
    class QCNetworkAccessManager;
    class QCNetworkSyncReply;
    class QCNetworkRequest;
}

namespace PhoenixPlayer{

    namespace DataProvider {

        namespace LastFmLookup {

//#define LASTFMAPIKEY "48570d887cca45f05f8094dca4f79c0d"
//#define API_BASE "http://ws.audioscrobbler.com/2.0"
//#define API_KEY "625fd47b3b685af19315cc3a1aa5920a"

///*
// *  macro based on last.fm json result
// *  should use a configuration later
// *
// */
//#define IMAGE_SIZE_REGEXP_LARGE "large"
//#define IMAGE_SIZE_REGEXP_MEDIA "medium"
//#define IMAGE_SIZE_REGEXP_SMALL "small"
//#define IMAGE_SIZE_REGEXP_EXTRA_LARGE "extralarge"
//#define IMAGE_SIZE_REGEXP_MEGA "mega"

//#define CONFIG_KEY "LastFM_Image_Size"

//#define DEFAULT_IMAGE_SIZE_REGEXP IMAGE_SIZE_REGEXP_MEDIA

class LIBPHOENIXPLAYER_EXPORT LastFmLookup : public IMetadataLookup
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.MetadataLookup.LastFmLookup" FILE "lastfmlookup.json")
    Q_INTERFACES(PhoenixPlayer::BasePlugin)
public:
    LastFmLookup(QObject *parent = Q_NULLPTR);
    virtual ~LastFmLookup() override;

    // IMetadataLookup interface
//public:
//    bool supportLookup(LookupType type);
//    void lookup(const AudioMetaObject &object);

    // BasePlugin interface
public:
    PluginProperty property() const Q_DECL_OVERRIDE;
    //TODO reimplement setttings ui later
    QWidget *settingsUI() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }
    // IDataProvider interface
public:
    bool support(SupportedTypes type) const Q_DECL_OVERRIDE;

    // IMetadataLookup interface
public:
    QByteArray lookup(const AudioMetaObject &object, SupportedType type, QString *errorMsg) const Q_DECL_OVERRIDE;

protected:
//    void parseAlbumDescription(const QByteArray &qba);
//    void parseAlbumImage(const QByteArray &qba);
//    void parseArtistDescription(const QByteArray &qba);
//    void parseArtistImage(const QByteArray &qba);
    QByteArray parseSongDescription(const QByteArray &qba) const;
    QByteArray parseAlbumData(const QByteArray &qba, SupportedType type) const;
    QByteArray parseArtisData(const QByteArray &qba, SupportedType type) const;

private:
    bool parseRootObject(QJsonObject &out, const QByteArray &in, const QString &key) const;
    QString formatStr(const QString &in) const;
    void setErrorMsg(QString *msgPtr, const QString &msgData) const;
    void setRequestHeader(QCurl::QCNetworkRequest *req) const;
    QByteArray networkCall(const QCurl::QCNetworkRequest &request, QString *errorMsg) const;

private:
//    BaseNetworkLookup *m_networkLookup;
    QCurl::QCNetworkAccessManager   *m_network  = Q_NULLPTR;
    QTimer                          *m_timer    = Q_NULLPTR;
    QString m_imageSizeValue;
};

} //LastFmLookup
} //DataProvider
} //PhoenixPlayer
#endif // LASTFMLOOKUP_H
