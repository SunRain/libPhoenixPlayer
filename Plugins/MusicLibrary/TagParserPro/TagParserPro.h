#ifndef TAGPARSERPRO_H
#define TAGPARSERPRO_H

#include "taglib/apefile.h"
#include "taglib/apetag.h"
#include "taglib/asffile.h"
#include "taglib/fileref.h"
#include "taglib/flacfile.h"
#include "taglib/id3v2tag.h"
#include "taglib/mpcfile.h"
#include "taglib/mp4file.h"
#include "taglib/mpegfile.h"
#include "taglib/vorbisfile.h"
#include "taglib/wavpackfile.h"

#include "MusicLibrary/IMusicTagParser.h"

class QImage;
namespace PhoenixPlayer {
class AudioMetaObject;
namespace MusicLibrary {

namespace TagParserPro {

class TagParserPro : public IMusicTagParser
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.MusicTagParser.TagParserPro" FILE "tagparserpro.json")
    Q_INTERFACES(PhoenixPlayer::MusicLibrary::IMusicTagParser)

public:
    TagParserPro(QObject *parent = 0);
    virtual ~TagParserPro();

    // IMusicTagParser interface
public:
    bool parserTag(AudioMetaObject *target);

protected:
    QImage getImage();
private:
    bool isValid();
    QImage fromTagBytes(const TagLib::ByteVector &data);
    QImage fromApe(TagLib::APE::Tag *tag);
    QImage fromAsf(TagLib::ASF::Tag *tag);
    QImage fromFlac(TagLib::FLAC::File *file);
    QImage fromId3(TagLib::ID3v2::Tag *tag);
    QImage fromMp4(TagLib::MP4::Tag *tag);
    QImage fromVorbis(TagLib::Tag *tag);

private:
//    bool isUrl(const QString &file);
private:
    TagLib::FileRef *m_tagRef;
    QString m_filePath;
};

} //TagParserPro
} //MusicLibrary
} //PhoenixPlayer

#endif // TAGPARSERPRO_H
