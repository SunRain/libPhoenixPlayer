#ifndef TAGPARSERID3V2_H
#define TAGPARSERID3V2_H
#include <QObject>
#include <QImage>
#include <QHash>
#include <QMap>
#include <QList>

#include "IMusicTagParser.h"

class QTextCodec;
class QImage;
namespace PhoenixPlayer {
class SongMetaData;
class Settings;
namespace MusicLibrary {
namespace TagParserID3v2 {

enum ID3v2HeaderFlag
{
    Unsynchronisation = 0b10000000,
    ExtendedHeader = 0b01000000,
    ExperimentalIndicator = 0b00100000
};
enum ID3v2FrameFlag
{
    FrameDataLengthIndicator = 0b00000001,
    FrameUnsynchronisation = 0b00000010
};
enum ID3v2TextEncoding
{
    EncodeISO,
    EncodeUTF16BELE,
    EncodeUTF16,
    EncodeUTF8
};
struct ID3v2Header
{
    quint8 major = 0;
    quint8 minor = 0;
    quint32 size = 0;
    quint8 flag = 0;
};
struct ID3v2Frame
{
    char frameID[5] = {0};
    char *start;
    quint32 size = 0;
    char flags[2] = {0};
};
struct ID3v2PictureFrame
{
    QString mimeType;
    QString description;
    QImage image;
};
typedef quint32 (*FrameSizeCalculator)(char *);
typedef void (*FlagSaver)(char *, ID3v2Frame &);
struct ID3v2MinorProperty
{
    int frameIDSize;
    int frameHeaderSize;
    FrameSizeCalculator toSize = nullptr;
    FlagSaver saveFlag = nullptr;
};

enum MusicDatas
{
    Name = 0x0,
    Album,
    AlbumArtist,
    AlbumRating,
    Artist,
    BeatsPerMinuate,
    BitRate,
    Category,
    Comments,
    Composer,
    DateAdded,
    DateModified,
    Description,
    DiscCount,
    DiscNumber,
    Genre,
    Kind,
    LastPlayed,
    Plays,
    Rating,
    SampleRate,
    Size,
    Time,
    TrackCount,
    TrackNumber,
    Year,
    MusicDataCount
};

struct ID3v2Item {
    SongMetaData *data;
    QImage coverImage;
    QMap<QString, QList<QByteArray>> imageData;
};

class TagParserID3v2 : public IMusicTagParser
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.MusicLibrary.TagParserID3v2" FILE "tagparserid3v2.json")
    Q_INTERFACES(PhoenixPlayer::MusicLibrary::IMusicTagParser)
public:
    TagParserID3v2(QObject *parent = 0);
    virtual ~TagParserID3v2();


    // IMusicTagParser interface
public:
    bool parserTag(SongMetaData *targetMetaDate);

protected:
    bool parseAlbumArt();
    QString frameToText(QByteArray content);
    bool usingDefaultCodec() const;
    void setUsingDefaultCodec(bool usingDefaultCodec);

protected:
    inline bool parseID3v2Header(char *rawHeader,
                                 ID3v2Header &header);

    inline void generateID3v2Property(const quint8 &minor,
                                      ID3v2MinorProperty &property);

    bool parseID3v2RawData(char *rawTagData,
                           const ID3v2Header &header,
                           const ID3v2MinorProperty &property,
                           QLinkedList<ID3v2Frame> &frameList);

    void writeID3v2ToDetails(const QLinkedList<ID3v2Frame> &frames,
                             const ID3v2MinorProperty &property,
                             /*KNMusicAnalysisItem &analysisItem*/
                             SongMetaData *data);


private:
    static inline int ratingStars(const quint8 &hex);
    static inline quint32 minor2Size(char *rawTagData);
    static inline quint32 minor3Size(char *rawTagData);
    static inline quint32 minor4Size(char *rawTagData);
    static inline void saveFlag(char *rawTagData, ID3v2Frame &frameData);

    inline void parseAPICImageData(QByteArray imageData,
                                   QHash<int, ID3v2PictureFrame> &imageMap);
    inline void parsePICImageData(QByteArray imageData,
                                  QHash<int, ID3v2PictureFrame> &imageMap);
    QHash<QString, int> mFrameIDIndex;
//    KNMusicGlobal *m_musicGlobal;

    QByteArray mUnSyncRaw;
    QByteArray mUnSyncTo;
    QTextCodec *m_isoCodec;
    QTextCodec *m_utf16BECodec;
    QTextCodec *m_utf16LECodec;
    QTextCodec *m_utf16Codec;
    QTextCodec *m_utf8Codec;
    QTextCodec *m_localeCodec;
    bool mUsingDefaultCodec = true;

    ID3v2Item mID3v2Item;
    Settings *mSettings;
};

} //TagParserID3v2
} //MusicLibrary
} //PhoenixPlayer


#endif // TAGPARSERID3V2_H
