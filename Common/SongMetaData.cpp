#include "SongMetaData.h"
namespace PhoenixPlayer {
SongMetaData::SongMetaData(QObject *parent) : QObject(parent)
{

}

SongMetaData::~SongMetaData()
{

}


QString SongMetaData::hash() const
{
    return mHash;
}

void SongMetaData::setHash(const QString &hash)
{
    mHash = hash;
}

QString SongMetaData::filePath() const
{
    return mFilePath;
}

void SongMetaData::setFilePath(const QString &filePath)
{
    mFilePath = filePath;
}

QString SongMetaData::fileName() const
{
    return mFileName;
}

void SongMetaData::setFileName(const QString &fileName)
{
    mFileName = fileName;
}

int SongMetaData::mediaBitrate() const
{
    return mMediaBitrate;
}

void SongMetaData::setMediaBitrate(int mediaBitrate)
{
    mMediaBitrate = mediaBitrate;
}

int SongMetaData::fileSize() const
{
    return mFileSize;
}

void SongMetaData::setFileSize(int fileSize)
{
    mFileSize = fileSize;
}
QString SongMetaData::artistName() const
{
    return mArtistName;
}

void SongMetaData::setArtistName(const QString &artistName)
{
    mArtistName = artistName;
}
QString SongMetaData::artistImageUri() const
{
    return mArtistImageUri;
}

void SongMetaData::setArtistImageUri(const QString &artistImageUri)
{
    mArtistImageUri = artistImageUri;
}
QString SongMetaData::artistDescription() const
{
    return mArtistDescription;
}

void SongMetaData::setArtistDescription(const QString &artistDescription)
{
    mArtistDescription = artistDescription;
}
QString SongMetaData::albumName() const
{
    return mAlbumName;
}

void SongMetaData::setAlbumName(const QString &albumName)
{
    mAlbumName = albumName;
}
QString SongMetaData::albumDescription() const
{
    return mAlbumDescription;
}

void SongMetaData::setAlbumDescription(const QString &albumDescription)
{
    mAlbumDescription = albumDescription;
}
QString SongMetaData::albumYear() const
{
    return mAlbumYear;
}

void SongMetaData::setAlbumYear(const QString &albumYear)
{
    mAlbumYear = albumYear;
}
QString SongMetaData::coverArtSmall() const
{
    return mCoverArtSmall;
}

void SongMetaData::setCoverArtSmall(const QString &coverArtSmall)
{
    mCoverArtSmall = coverArtSmall;
}
QString SongMetaData::coverArtLarge() const
{
    return mCoverArtLarge;
}

void SongMetaData::setCoverArtLarge(const QString &coverArtLarge)
{
    mCoverArtLarge = coverArtLarge;
}
QString SongMetaData::coverArtMiddle() const
{
    return mCoverArtMiddle;
}

void SongMetaData::setCoverArtMiddle(const QString &coverArtMiddle)
{
    mCoverArtMiddle = coverArtMiddle;
}
Common::MediaType SongMetaData::mediaType() const
{
    return mMediaType;
}

void SongMetaData::setMediaType(const Common::MediaType &mediaType)
{
    mMediaType = mediaType;
}
int SongMetaData::songLength() const
{
    return mSongLength;
}

void SongMetaData::setSongLength(int songLength)
{
    mSongLength = songLength;
}
QString SongMetaData::songTitle() const
{
    return mSongTitle;
}

void SongMetaData::setSongTitle(const QString &songTitle)
{
    mSongTitle = songTitle;
}
QString SongMetaData::songDescription() const
{
    return mSongDescription;
}

void SongMetaData::setSongDescription(const QString &songDescription)
{
    mSongDescription = songDescription;
}
QStringList SongMetaData::category() const
{
    return mCategory;
}

void SongMetaData::setCategory(const QStringList &category)
{
    mCategory = category;
}
int SongMetaData::year() const
{
    return mYear;
}

void SongMetaData::setYear(int year)
{
    mYear = year;
}
QDate SongMetaData::date() const
{
    return mDate;
}

void SongMetaData::setDate(const QDate &date)
{
    mDate = date;
}
int SongMetaData::userRating() const
{
    return mUserRating;
}

void SongMetaData::setUserRating(int userRating)
{
    mUserRating = userRating;
}
QStringList SongMetaData::keywords() const
{
    return mKeywords;
}

void SongMetaData::setKeywords(const QStringList &keywords)
{
    mKeywords = keywords;
}
QString SongMetaData::language() const
{
    return mLanguage;
}

void SongMetaData::setLanguage(const QString &language)
{
    mLanguage = language;
}
QString SongMetaData::publisher() const
{
    return mPublisher;
}

void SongMetaData::setPublisher(const QString &publisher)
{
    mPublisher = publisher;
}
QString SongMetaData::copyright() const
{
    return mCopyright;
}

void SongMetaData::setCopyright(const QString &copyright)
{
    mCopyright = copyright;
}
QString SongMetaData::mood() const
{
    return mMood;
}

void SongMetaData::setMood(const QString &mood)
{
    mMood = mood;
}
























}//PhoenixPlayer
