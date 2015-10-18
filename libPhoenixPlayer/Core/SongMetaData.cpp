#include <QDebug>

#include "Util.h"
#include "SongMetaData.h"
#include "Common.h"

namespace PhoenixPlayer {

using namespace MetaData;

SongMetaData::SongMetaData(const QString &path, const QString &name, quint64 size, QObject *parent)
    : BaseObject(parent)
    , m_path(path)
    , m_name(name)
    , m_size(size)
{
    m_mediaType = (int)Common::MediaTypeLocalFile;
    m_hash = SongMetaData::formatHash(path, name, size);

    m_albumMeta = new AlbumMeta(this);
    m_artistMeta = new ArtistMeta(this);
    m_coverMeta = new CoverMeta(this);
    m_trackMeta = new TrackMeta(this);
}

SongMetaData::SongMetaData(const SongMetaData *other, QObject *parent)
    : BaseObject(parent)
{
    if (other) {
        m_path = other->hash ();
        m_name = other->name ();
        m_hash = other->hash ();
        m_size = other->size ();
        m_mediaType = other->mediaType ();
        m_lyricsData = other->lyricsData ();
        m_lyricsUri = other->lyricsUri ();
        m_albumMeta = new AlbumMeta(other->albumMeta (), this);
        m_artistMeta = new ArtistMeta(other->artistMeta (), this);
        m_coverMeta = new CoverMeta(other->coverMeta (), this);
        m_trackMeta = new TrackMeta(other->trackMeta (), this);
    } else {
        m_path = QString();
        m_name = QString();
        m_hash = QString();
        m_size = 0;
        m_mediaType = (int)Common::MediaTypeLocalFile;
        m_albumMeta = new AlbumMeta(this);
        m_artistMeta = new ArtistMeta(this);
        m_coverMeta = new CoverMeta(this);
        m_trackMeta = new TrackMeta(this);
    }
}

SongMetaData::SongMetaData(SongMetaData **other, QObject *parent)
    : BaseObject(parent)
{
    if (other) {
        m_path = (*other)->hash ();
        m_name = (*other)->name ();
        m_hash = (*other)->hash ();
        m_size = (*other)->size ();
        m_mediaType = (*other)->mediaType ();
        m_lyricsData = (*other)->lyricsData ();
        m_lyricsUri = (*other)->lyricsUri ();
        m_albumMeta = new AlbumMeta((*other)->albumMeta (), this);
        m_artistMeta = new ArtistMeta((*other)->artistMeta (), this);
        m_coverMeta = new CoverMeta((*other)->coverMeta (), this);
        m_trackMeta = new TrackMeta((*other)->trackMeta (), this);
    } else {
        m_path = QString();
        m_name = QString();
        m_hash = QString();
        m_size = 0;
        m_mediaType = (int)Common::MediaTypeLocalFile;
        m_albumMeta = new AlbumMeta(this);
        m_artistMeta = new ArtistMeta(this);
        m_coverMeta = new CoverMeta(this);
        m_trackMeta = new TrackMeta(this);
    }
}

SongMetaData::SongMetaData(const QUrl &url, QObject *parent)
    : BaseObject(parent)
{
    m_mediaType = (int)Common::MediaTypeUrl;
    m_path = url.toString ();
    m_hash = Util::calculateHash (url.toString ());
    m_size = 0;
    m_albumMeta = new AlbumMeta(this);
    m_artistMeta = new ArtistMeta(this);
    m_coverMeta = new CoverMeta(this);
    m_trackMeta = new TrackMeta(this);
}

SongMetaData::~SongMetaData()
{

}

QString SongMetaData::toString()
{
    QStringList values;
    QStringList props = m_albumMeta->propertyList ();
    props.removeDuplicates ();
    foreach (QString s, props) {
        QString v = QString("%1 = [%2]")
                .arg (s)
                .arg (m_albumMeta->property (s.toLocal8Bit ().constData ()).toString ());
        values.append (v);
    }

    props.clear ();
    props = m_artistMeta->propertyList ();
    props.removeDuplicates ();
    foreach (QString s, props) {
        QString v = QString("%1 = [%2]")
                .arg (s)
                .arg (m_artistMeta->property (s.toLocal8Bit ().constData ()).toString ());
        values.append (v);
    }

    props.clear ();
    props = m_coverMeta->propertyList ();
    props.removeDuplicates ();
    foreach (QString s, props) {
        QString v = QString("%1 = [%2]")
                .arg (s)
                .arg (m_coverMeta->property (s.toLocal8Bit ().constData ()).toString ());
        values.append (v);
    }

    props.clear ();
    props = m_trackMeta->propertyList ();
    props.removeDuplicates ();
    foreach (QString s, props) {
        QString v = QString("%1 = [%2]")
                .arg (s)
                .arg (m_trackMeta->property (s.toLocal8Bit ().constData ()).toString ());
        values.append (v);
    }

    props.clear ();
    props = this->propertyList ();
    props.removeDuplicates ();
    foreach (QString s, props) {
        QString v = QString("%1 = [%2]")
                .arg (s)
                .arg (this->property (s.toLocal8Bit ().constData ()).toString ());
        values.append (v);
    }
    return values.join (",");
}

bool SongMetaData::equals(const SongMetaData *other)
{
    return m_hash == other->hash ();
}

void SongMetaData::fillAttribute(const SongMetaData *other)
{
    if (!other) {
        qDebug()<<Q_FUNC_INFO<<"No other pointer";
        return;
    }
    if (m_hash != other->hash ()) {
        qDebug()<<Q_FUNC_INFO<<"hash invalid";
        return;
    }
    m_path = other->path ();
    m_name = other->name ();
    m_size = other->size ();
    m_mediaType = other->mediaType ();
    m_lyricsData = other->lyricsData ();
    m_lyricsUri = other->lyricsUri ();
    m_albumMeta->setDate (other->albumMeta ()->date ());
    m_albumMeta->setDescription (other->albumMeta ()->description ());
    m_albumMeta->setImgUri (other->albumMeta ()->imgUri ());
    m_albumMeta->setName (other->albumMeta ()->name ());
    m_artistMeta->setDescription (other->artistMeta ()->description ());
    m_artistMeta->setImgUri (other->artistMeta ()->imgUri ());
    m_artistMeta->setName (other->artistMeta ()->name ());
    m_coverMeta->setLargeUri (other->coverMeta ()->largeUri ());
    m_coverMeta->setMiddleUri (other->coverMeta ()->middleUri ());
    m_coverMeta->setSmallUri (other->coverMeta ()->smallUri ());
    m_trackMeta->setBitRate (other->trackMeta ()->bitRate ());
    m_trackMeta->setDate (other->trackMeta ()->date ());
    m_trackMeta->setDescription (other->trackMeta ()->description ());
    m_trackMeta->setDuration (other->trackMeta ()->duration ());
    m_trackMeta->setGenre (other->trackMeta ()->genre ());
    m_trackMeta->setSampleRate (other->trackMeta ()->sampleRate ());
    m_trackMeta->setTitle (other->trackMeta ()->title ());
    m_trackMeta->setUserRating (other->trackMeta ()->userRating ());
    m_trackMeta->setYear (other->trackMeta ()->year ());
}

//QStringList SongMetaData::metaKeys()
//{
//    QStringList list;
//    list.append (ArtistMeta::staticPropertyList ());
//    list.append (AlbumMeta::staticPropertyList ());
//    list.append (CoverMeta::staticPropertyList ());
//    list.append (TrackMeta::staticPropertyList ());
//    list.append (SongMetaData::staticPropertyList ());
//    list.removeDuplicates ();
//    return list;
//}

QString SongMetaData::formatHash(const QString &path, const QString &name, quint64 size)
{
    return Util::calculateHash (QString("%1/%2-%3").arg (path).arg (name).arg (size));
}

QString SongMetaData::hash() const
{
    return m_hash;
}

QString SongMetaData::path() const
{
    return m_path;
}

QString SongMetaData::name() const
{
    return m_name;
}

quint64 SongMetaData::size() const
{
    return m_size;
}

int SongMetaData::mediaType() const
{
    return m_mediaType;
}

QString SongMetaData::lyricsData() const
{
    return m_lyricsData;
}

QUrl SongMetaData::lyricsUri() const
{
    return m_lyricsUri;
}

inline AlbumMeta *SongMetaData::albumMeta() const
{
    return m_albumMeta;
}

inline ArtistMeta *SongMetaData::artistMeta() const
{
    return m_artistMeta;
}

inline CoverMeta *SongMetaData::coverMeta() const
{
    return m_coverMeta;
}

inline TrackMeta *SongMetaData::trackMeta() const
{
    return m_trackMeta;
}

QUrl SongMetaData::uri() const
{
    if (m_mediaType == (int)Common::MediaTypeLocalFile) {
        if (!m_path.isEmpty () && !m_name.isEmpty ())
            //TODO file:// or file:/ ? or no file:/ (file://) ?
            //FIXME should fix path like ../path/to/file
            return QUrl::fromLocalFile (QString("%1/%2").arg (m_path).arg (m_name));
    } else {
        if (!m_path.isEmpty ())
            return QUrl(m_path);
        else if (!m_name.isEmpty ())
            return QUrl(m_name);
    }
    return QUrl();
}

QUrl SongMetaData::queryImgUri() const
{
    if (coverMeta ()) {
        if (!m_coverMeta->middleUri ().isEmpty ())
            return m_coverMeta->middleUri ();
        if (!m_coverMeta->largeUri ().isEmpty ())
            return m_coverMeta->largeUri ();
        if (!m_coverMeta->smallUri ().isEmpty ())
            return m_coverMeta->smallUri ();
    }
    if (artistMeta ()) {
        if (!artistMeta ()->imgUri ().isEmpty ())
            return artistMeta ()->imgUri ();
    }
    if (albumMeta ()) {
        if (!albumMeta ()->imgUri ().isEmpty ())
            return albumMeta ()->imgUri ();
    }
    return QUrl();
}

//void SongMetaData::setSize(QVariant arg)
//{
//    if (m_size != arg) {
//        m_size = arg;
//        emit sizeChanged(arg);
//    }
//}

void SongMetaData::setMediaType(int arg)
{
    if (m_mediaType != arg) {
        m_mediaType = arg;
        emit mediaTypeChanged(arg);
    }
}

void SongMetaData::setLyricsData(QString arg)
{
    if (m_lyricsData != arg) {
        m_lyricsData = arg;
        emit lyricsDataChanged(arg);
    }
}

void SongMetaData::setLyricsUri(QUrl arg)
{
    if (m_lyricsUri != arg) {
        m_lyricsUri = arg;
        emit lyricsUriChanged(arg);
    }
}

///////////////////////  namespace MetaData /////////////////////////////////
namespace MetaData {

AlbumMeta::AlbumMeta(QObject *parent)
    : BaseObject(parent)
{
}

AlbumMeta::AlbumMeta(const AlbumMeta *other, QObject *parent)
    : BaseObject(parent)
{
    m_name = other->name ();
    m_imgUri = other->imgUri ();
    m_description = other->description ();
    m_date = other->date ();
}

AlbumMeta::~AlbumMeta()
{

}

QString AlbumMeta::name() const
{
    return m_name;
}

QUrl AlbumMeta::imgUri() const
{
    return m_imgUri;
}

QString AlbumMeta::description() const
{
    return m_description;
}

QVariant AlbumMeta::date() const
{
    return m_date;
}

void AlbumMeta::setName(QString arg)
{
    if (m_name != arg) {
        m_name = arg;
        emit nameChanged(arg);
    }
}

void AlbumMeta::setImgUri(QUrl arg)
{
    if (m_imgUri != arg) {
        m_imgUri = arg;
        emit imgUriChanged(arg);
    }
}

void AlbumMeta::setDescription(QString arg)
{
    if (m_description != arg) {
        m_description = arg;
        emit descriptionChanged(arg);
    }
}

void AlbumMeta::setDate(QVariant arg)
{
    if (m_date != arg) {
        m_date = arg;
        emit dateChanged(arg);
    }
}

ArtistMeta::ArtistMeta(QObject *parent)
    :BaseObject(parent)
{
}

ArtistMeta::ArtistMeta(const ArtistMeta *other, QObject *parent)
    :BaseObject(parent)
{
    m_name = other->name ();
    m_imgUri = other->imgUri ();
    m_description = other->description ();
}

ArtistMeta::~ArtistMeta()
{

}

QString ArtistMeta::name() const
{
    return m_name;
}

QUrl ArtistMeta::imgUri() const
{
    return m_imgUri;
}

QString ArtistMeta::description() const
{
    return m_description;
}

void ArtistMeta::setName(QString arg)
{
    if (m_name != arg) {
        m_name = arg;
        emit nameChanged(arg);
    }
}

void ArtistMeta::setImgUri(QUrl arg)
{
    if (m_imgUri != arg) {
        m_imgUri = arg;
        emit imgUriChanged(arg);
    }
}

void ArtistMeta::setDescription(QString arg)
{
    if (m_description != arg) {
        m_description = arg;
        emit descriptionChanged(arg);
    }
}

CoverMeta::CoverMeta(QObject *parent)
    :BaseObject(parent)
{

}

CoverMeta::CoverMeta(const CoverMeta *other, QObject *parent)
    :BaseObject(parent)
{
    m_smallUri = other->smallUri ();
    m_middleUri = other->middleUri ();
    m_largeUri = other->largeUri ();
}

CoverMeta::~CoverMeta()
{

}

QUrl CoverMeta::smallUri() const
{
    return m_smallUri;
}

QUrl CoverMeta::middleUri() const
{
    return m_middleUri;
}

QUrl CoverMeta::largeUri() const
{
    return m_largeUri;
}

void CoverMeta::setSmallUri(QUrl arg)
{
    if (m_smallUri != arg) {
        m_smallUri = arg;
        emit smallUriChanged(arg);
    }
}

void CoverMeta::setMiddleUri(QUrl arg)
{
    if (m_middleUri != arg) {
        m_middleUri = arg;
        emit middleUriChanged(arg);
    }
}

void CoverMeta::setLargeUri(QUrl arg)
{
    if (m_largeUri != arg) {
        m_largeUri = arg;
        emit largeUriChanged(arg);
    }
}

TrackMeta::TrackMeta(QObject *parent)
    :BaseObject(parent)
{

}

TrackMeta::TrackMeta(const TrackMeta *other, QObject *parent)
    :BaseObject(parent)
{
    m_bitRate = other->bitRate ();
    m_date = other->date ();
    m_description = other->description ();
    m_duration = other->duration ();
    m_genre = other->genre ();
    m_sampleRate = other->sampleRate ();
    m_title = other->title ();
    m_userRating = other->userRating ();
    m_year = other->year ();
}

TrackMeta::~TrackMeta()
{

}

QVariant TrackMeta::bitRate() const
{
    return m_bitRate;
}

int TrackMeta::duration() const
{
    return m_duration;
}

QString TrackMeta::title() const
{
    return m_title;
}

QString TrackMeta::description() const
{
    return m_description;
}

QVariant TrackMeta::year() const
{
    return m_year;
}

QVariant TrackMeta::date() const
{
    return m_date;
}

QVariant TrackMeta::genre() const
{
    return m_genre;
}

QVariant TrackMeta::sampleRate() const
{
    return m_sampleRate;
}

QVariant TrackMeta::userRating() const
{
    return m_userRating;
}

void TrackMeta::setBitRate(QVariant arg)
{
    if (m_bitRate != arg) {
        m_bitRate = arg;
        emit bitRateChanged(arg);
    }
}

void TrackMeta::setDuration(int arg)
{
    if (m_duration != arg) {
        m_duration = arg;
        emit durationChanged(arg);
    }
}

void TrackMeta::setTitle(QString arg)
{
    if (m_title != arg) {
        m_title = arg;
        emit titleChanged(arg);
    }
}

void TrackMeta::setDescription(QString arg)
{
    if (m_description != arg) {
        m_description = arg;
        emit descriptionChanged(arg);
    }
}

void TrackMeta::setYear(QVariant arg)
{
    if (m_year != arg) {
        m_year = arg;
        emit yearChanged(arg);
    }
}

void TrackMeta::setDate(QVariant arg)
{
    if (m_date != arg) {
        m_date = arg;
        emit dateChanged(arg);
    }
}

void TrackMeta::setGenre(QVariant arg)
{
    if (m_genre != arg) {
        m_genre = arg;
        emit genreChanged(arg);
    }
}

void TrackMeta::setSampleRate(QVariant arg)
{
    if (m_sampleRate != arg) {
        m_sampleRate = arg;
        emit sampleRateChanged(arg);
    }
}

void TrackMeta::setUserRating(QVariant arg)
{
    if (m_userRating != arg) {
        m_userRating = arg;
        emit userRatingChanged(arg);
    }
}

} //MetaData
} //PhoenixPlayer
