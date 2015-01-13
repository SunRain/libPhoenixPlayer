#ifndef SONGMETADATA_H
#define SONGMETADATA_H

#include <QObject>
#include <QStringList>
#include <QDate>
#include "Common.h"

class QStringList;
class QDate;
namespace PhoenixPlayer {

class SongMetaData : public QObject
{
    Q_OBJECT
public:
    explicit SongMetaData(QObject *parent = 0);
    virtual ~SongMetaData();

    QString hash() const;
    void setHash(const QString &hash);

    QString filePath() const;
    void setFilePath(const QString &filePath);

    QString fileName() const;
    void setFileName(const QString &fileName);

    int mediaBitrate() const;
    void setMediaBitrate(int mediaBitrate);

    int fileSize() const;
    void setFileSize(int fileSize);

    QString artistName() const;
    void setArtistName(const QString &artistName);

    QString artistImageUri() const;
    void setArtistImageUri(const QString &artistImageUri);

    QString artistDescription() const;
    void setArtistDescription(const QString &artistDescription);

    QString albumName() const;
    void setAlbumName(const QString &albumName);

    QString albumDescription() const;
    void setAlbumDescription(const QString &albumDescription);

    QString albumYear() const;
    void setAlbumYear(const QString &albumYear);

    QString coverArtSmall() const;
    void setCoverArtSmall(const QString &coverArtSmall);

    QString coverArtLarge() const;
    void setCoverArtLarge(const QString &coverArtLarge);

    QString coverArtMiddle() const;
    void setCoverArtMiddle(const QString &coverArtMiddle);

    Common::MediaType mediaType() const;
    void setMediaType(const Common::MediaType &mediaType);

    int songLength() const;
    void setSongLength(int songLength);

    QString songTitle() const;
    void setSongTitle(const QString &songTitle);

    QString songDescription() const;
    void setSongDescription(const QString &songDescription);

    QStringList category() const;
    void setCategory(const QStringList &category);

    int year() const;
    void setYear(int year);

    QDate date() const;
    void setDate(const QDate &date);

    int userRating() const;
    void setUserRating(int userRating);

    QStringList keywords() const;
    void setKeywords(const QStringList &keywords);

    QString language() const;
    void setLanguage(const QString &language);

    QString publisher() const;
    void setPublisher(const QString &publisher);

    QString copyright() const;
    void setCopyright(const QString &copyright);

    QString mood() const;
    void setMood(const QString &mood);

private:
    QString mHash;
    QString mFilePath;
    QString mFileName;
    ///
    /// \brief mMediaBitrate 码率
    ///
    int mMediaBitrate;
    ///
    /// \brief mFileSize 文件大小
    ///
    int mFileSize;

    //作者信息
    QString mArtistName; //作者姓名
    ///
    /// \brief mArtistImageUri 图片地址或者链接
    ///
    QString mArtistImageUri;
    QString mArtistDescription ;//介绍

    // 唱片信息
    QString mAlbumName;
//    QString mAlbumImageUrl;
    QString mAlbumDescription; //介绍
    QString mAlbumYear; //唱片发型日期

    ///
    /// \brief The URL of a small cover art image. 图片地址或者链接
    ///
    QString mCoverArtSmall;
    ///
    /// \brief The URL of a large cover art image 图片地址或者链接
    ///
    QString mCoverArtLarge;
    ///
    /// \brief An middle cover art image 图片地址或者链接
    ///
    QString mCoverArtMiddle;

    Common::MediaType mMediaType;

    ///
    /// \brief mSongLength 歌曲长度
    ///
    int mSongLength;
    QString mSongTitle;
    QString mSongDescription;//	A description of the media.	QString
    QStringList mCategory; //	The category of the media.	QStringList
    int mYear;	//The year of release of the media.	int
    QDate mDate; //	The date of the media.	QDate.
    int mUserRating; //	A user rating of the media.	int [0..100]
    QStringList mKeywords;//	A list of keywords describing the media.	QStringList
    QString mLanguage;  //The language of media, as an ISO 639-2 code.	QString
    QString mPublisher;	//The publisher of the media.	QString
    QString mCopyright;	//The media's copyright notice.	QString
    //ParentalRating	The parental rating of the media.	QString
    //RatingOrganization	The organization responsible for the parental rating of the media.	QString

//    AlbumTitle	The title of the album the media belongs to.	QString
//    AlbumArtist	The principal artist of the album the media belongs to.	QString
//    ContributingArtist	The artists contributing to the media.	QStringList
//    Composer	The composer of the media.	QStringList
//    Conductor	The conductor of the media.	QString
//    QString mLyrics;	//he lyrics to the media.	QString
    QString mMood;  //	The mood of the media.	QString
//    TrackNumber	The track number of the media.	int
//    TrackCount	The number of tracks on the album containing the media.	int
};

} //PhoenixPlayer
#endif // SONGMETADATA_H
