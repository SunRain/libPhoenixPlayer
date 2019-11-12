#ifndef TAGPARSERPRO_H
#define TAGPARSERPRO_H

#include <QImage>

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

#include "DataProvider/IMusicTagParser.h"

namespace PhoenixPlayer {

namespace DataProvider {

namespace TagParserPro {

class TagParserPro : public IMusicTagParser
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.MusicTagParser.TagParserPro" FILE "tagparserpro.json")
    Q_INTERFACES(PhoenixPlayer::BasePlugin)

public:
    TagParserPro(QObject *parent = Q_NULLPTR);
    virtual ~TagParserPro() override;

    // IMusicTagParser interface
public:
    bool parserTag(AudioMetaObject *target) Q_DECL_OVERRIDE;

    // BasePlugin interface
public:
    PluginProperty property() const Q_DECL_OVERRIDE;

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
    TagLib::FileRef *m_tagRef = Q_NULLPTR;
    QString         m_filePath;
};

} //TagParserPro
} //DataProvider
} //PhoenixPlayer

#endif // TAGPARSERPRO_H
