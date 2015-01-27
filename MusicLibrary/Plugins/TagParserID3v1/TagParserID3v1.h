#ifndef TAGPARSERID3V1_H
#define TAGPARSERID3V1_H

#include <QObject>

#include "MusicLibrary/IMusicTagParser.h"

class QTextCodec;
namespace PhoenixPlayer {
class SongMetaData;
namespace MusicLibrary {
namespace TagParserID3v1 {

struct ID3v1Struct
{
    QString tags[5];
    int track=-1;
    int genreIndex=-1;
};

class TagParserID3v1 : public IMusicTagParser
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.MusicLibrary.TagParserID3v1" FILE "tagparserid3v1.json")
    Q_INTERFACES(PhoenixPlayer::MusicLibrary::IMusicTagParser)
public:
    TagParserID3v1(QObject *parent = 0);
    virtual ~TagParserID3v1();

    // IMusicTagParser interface
public:
    bool parserTag(SongMetaData *targetMetaDate);

private:
    inline void parseRawData(char *rawTagData, ID3v1Struct &tagData);
    inline QString standardizeText(const QString &text);
    inline void backupByte(char *rawTagData,
                           const int &backupPosition,
                           char &backupPool,
                           int &positionPool,
                           const bool &clearData=false);
    QTextCodec *mDefaultCodec;
};

} //TagParserID3v1
} //MusicLibrary
} //PhoenixPlayer

#endif // TAGPARSERID3V1_H
