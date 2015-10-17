#ifndef SONGMETADATA_H
#define SONGMETADATA_H

#include <QObject>
#include <QHash>
#include <QVariant>
#include <QDate>
#include <QUrl>
//#include "Common.h"
#include "BaseObject.h"

class QStringList;
class QDate;
namespace PhoenixPlayer {

namespace MetaData {

class AlbumMeta : public BaseObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QUrl imgUri READ imgUri WRITE setImgUri NOTIFY imgUriChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QVariant date READ date WRITE setDate NOTIFY dateChanged)
    DECLARE_STATIC_PROPERTY_LIST(MetaData::AlbumMeta)
public:
    explicit AlbumMeta(QObject *parent = 0);
    explicit AlbumMeta(const AlbumMeta *other, QObject *parent = 0);
    virtual ~AlbumMeta();
    QString name() const;
    QUrl imgUri() const;
    QString description() const;
    QVariant date() const;

public slots:
    void setName(QString arg);
    void setImgUri(QUrl arg);
    void setDescription(QString arg);
    void setDate(QVariant arg);
signals:
    void nameChanged(QString arg);
    void imgUriChanged(QUrl arg);
    void descriptionChanged(QString arg);
    void dateChanged(QVariant arg);
private:
    QString m_name;
    QUrl m_imgUri;
    QString m_description;
    QVariant m_date;
};

class ArtistMeta : public BaseObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QUrl imgUri READ imgUri WRITE setImgUri NOTIFY imgUriChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    DECLARE_STATIC_PROPERTY_LIST(MetaData::ArtistMeta)
public:
    explicit ArtistMeta(QObject *parent = 0);
    explicit ArtistMeta(const ArtistMeta *other, QObject *parent = 0);
    virtual ~ArtistMeta();
    QString name() const;
    QUrl imgUri() const;
    QString description() const;
public slots:
    void setName(QString arg);
    void setImgUri(QUrl arg);
    void setDescription(QString arg);
signals:
    void nameChanged(QString arg);
    void imgUriChanged(QUrl arg);
    void descriptionChanged(QString arg);
private:
    QString m_name;
    QUrl m_imgUri;
    QString m_description;
};

class CoverMeta : public BaseObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl smallUri READ smallUri WRITE setSmallUri NOTIFY smallUriChanged)
    Q_PROPERTY(QUrl middleUri READ middleUri WRITE setMiddleUri NOTIFY middleUriChanged)
    Q_PROPERTY(QUrl largeUri READ largeUri WRITE setLargeUri NOTIFY largeUriChanged)
    DECLARE_STATIC_PROPERTY_LIST(MetaData::CoverMeta)
public:
    explicit CoverMeta(QObject *parent = 0);
    explicit CoverMeta(const CoverMeta *other, QObject *parent = 0);
    virtual ~CoverMeta();
    QUrl smallUri() const;
    QUrl middleUri() const;
    QUrl largeUri() const;
public slots:
    void setSmallUri(QUrl arg);
    void setMiddleUri(QUrl arg);
    void setLargeUri(QUrl arg);
signals:
    void smallUriChanged(QUrl arg);
    void middleUriChanged(QUrl arg);
    void largeUriChanged(QUrl arg);
private:
    QUrl m_smallUri;
    QUrl m_middleUri;
    QUrl m_largeUri;
};

//    E_Category,
//    E_Keywords,
//    E_Language,
//    E_Publisher,
//    E_Copyright,
//    E_Mood,
//    E_Composer,
//    E_Conductor	,
//    E_Comment,
class TrackMeta : public BaseObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant bitRate READ bitRate WRITE setBitRate NOTIFY bitRateChanged)
    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QVariant year READ year WRITE setYear NOTIFY yearChanged)
    Q_PROPERTY(QVariant date READ date WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(QVariant genre READ genre WRITE setGenre NOTIFY genreChanged)
    Q_PROPERTY(QVariant sampleRate READ sampleRate WRITE setSampleRate NOTIFY sampleRateChanged)
    Q_PROPERTY(QVariant userRating READ userRating WRITE setUserRating NOTIFY userRatingChanged)
    DECLARE_STATIC_PROPERTY_LIST(MetaData::TrackMeta)
public:
    explicit TrackMeta(QObject *parent = 0);
    explicit TrackMeta(const TrackMeta *other, QObject *parent = 0);
    virtual ~TrackMeta();
    QVariant bitRate() const;
    int duration() const;
    QString title() const;
    QString description() const;
    QVariant year() const;
    QVariant date() const;
    QVariant genre() const;
    QVariant sampleRate() const;
    QVariant userRating() const;
public slots:
    void setBitRate(QVariant arg);
    void setDuration(int arg);
    void setTitle(QString arg);
    void setDescription(QString arg);
    void setYear(QVariant arg);
    void setDate(QVariant arg);
    void setGenre(QVariant arg);
    void setSampleRate(QVariant arg);
    void setUserRating(QVariant arg);
signals:
    void bitRateChanged(QVariant arg);
    void durationChanged(int arg);
    void titleChanged(QString arg);
    void descriptionChanged(QString arg);
    void yearChanged(QVariant arg);
    void dateChanged(QVariant arg);
    void genreChanged(QVariant arg);
    void sampleRateChanged(QVariant arg);
    void userRatingChanged(QVariant arg);
private:
    QVariant m_bitRate;
    int m_duration;
    QString m_title;
    QString m_description;
    QVariant m_year;
    QVariant m_date;
    QVariant m_genre;
    QVariant m_sampleRate;
    QVariant m_userRating;
};
} //MetaData

//typedef QList<SongMetaData *> SongMetaDataList;

class SongMetaData : public BaseObject
{
    Q_OBJECT
    Q_PROPERTY(QString hash READ hash CONSTANT)
    Q_PROPERTY(QString path READ path CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(quint64 size READ size CONSTANT)
    Q_PROPERTY(QUrl uri READ uri CONSTANT)
    Q_PROPERTY(int mediaType READ mediaType WRITE setMediaType NOTIFY mediaTypeChanged)
    Q_PROPERTY(QString lyricsData READ lyricsData WRITE setLyricsData NOTIFY lyricsDataChanged)
    Q_PROPERTY(QUrl lyricsUri READ lyricsUri WRITE setLyricsUri NOTIFY lyricsUriChanged)
    Q_PROPERTY(QUrl queryImgUri READ queryImgUri CONSTANT)

    Q_PROPERTY(MetaData::AlbumMeta* albumMeta READ albumMeta)
    Q_PROPERTY(MetaData::ArtistMeta* artistMeta READ artistMeta)
    Q_PROPERTY(MetaData::CoverMeta* coverMeta READ coverMeta)
    Q_PROPERTY(MetaData::TrackMeta* trackMeta READ trackMeta)

    DECLARE_STATIC_PROPERTY_LIST(SongMetaData)
public:
    explicit SongMetaData(const QString &path, const QString &name, quint64 size,QObject *parent = 0);
    explicit SongMetaData(const SongMetaData *other, QObject *parent = 0);
    explicit SongMetaData(SongMetaData **other, QObject *parent = 0);
    explicit SongMetaData(const QUrl &url, QObject *parent = 0);
    virtual ~SongMetaData();

    QString toString();
    bool equals(const SongMetaData *other);
    void fillAttribute(const SongMetaData *other);

    static QString formatHash(const QString &path, const QString &name, quint64 size);

    QString hash() const;
    QString path() const;
    QString name() const;
    quint64 size() const;
    int mediaType() const;
    QString lyricsData() const;
    QUrl lyricsUri() const;

    MetaData::AlbumMeta* albumMeta() const;
    MetaData::ArtistMeta* artistMeta() const;
    MetaData::CoverMeta* coverMeta() const;
    MetaData::TrackMeta* trackMeta() const;

    ///
    /// \brief uri
    /// \return 以file://开头的本地文件路径，或者网页地址
    ///
    QUrl uri() const;

    ///
    /// \brief 遍历所有属性，得到一个不为空的image地址
    /// \return
    ///
    QUrl queryImgUri() const;

public slots:
//    void setSize(QVariant arg);
    void setMediaType(int arg);
    void setLyricsData(QString arg);
    void setLyricsUri(QUrl arg);
signals:
//    void sizeChanged(QVariant arg);
    void mediaTypeChanged(int arg);
    void lyricsDataChanged(QString arg);
    void lyricsUriChanged(QUrl arg);
private:
    QString m_hash;
    QString m_path;
    QString m_name;
    quint64 m_size;
    int m_mediaType;
    QString m_lyricsData;
    QUrl m_lyricsUri;

    MetaData::AlbumMeta *m_albumMeta;
    MetaData::ArtistMeta *m_artistMeta;
    MetaData::CoverMeta *m_coverMeta;
    MetaData::TrackMeta *m_trackMeta;
};

} //PhoenixPlayer
#endif // SONGMETADATA_H
