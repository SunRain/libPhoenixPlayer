
#include <QFile>
#include <QImage>

#include "TagParserPro.h"
#include "SongMetaData.h"
#include "Settings.h"

namespace PhoenixPlayer {
class SongMetaData;
namespace MusicLibrary {

namespace TagParserPro {


TagParserPro::TagParserPro(QObject *parent)
    : IMusicTagParser(parent)
{
    mTagRef = nullptr;
}

TagParserPro::~TagParserPro()
{
    if (mTagRef != nullptr) {
        delete mTagRef;
        mTagRef = nullptr;
    }
}

bool TagParserPro::parserTag(SongMetaData *targetMetaDate)
{
    if (targetMetaDate == nullptr)
        return false;

    QString name = targetMetaDate->getMeta (Common::E_FileName).toString ();
    QString path = targetMetaDate->getMeta (Common::E_FilePath).toString ();
    mFilePath = QString("%1/%2").arg (path).arg (name);

    QFile f(mFilePath);
    if (!f.exists ())
        return false;

    mTagRef = new TagLib::FileRef(mFilePath.toLocal8Bit ().constData ());

    if (!isValid ())
        return false;

    TagLib::Tag *tag = mTagRef->tag ();
    QString str;
    if (tag) {
        str = TStringToQString(tag->artist());
        if (!str.isEmpty ()) {
            targetMetaDate->setMeta (Common::E_ArtistName, QVariant(str));
        }

        str = TStringToQString(tag->title());
        if (!str.isEmpty ()) {
            targetMetaDate->setMeta (Common::E_SongTitle, QVariant(str));
        }

        str = TStringToQString(tag->album());
        if (!str.isEmpty ()) {
            targetMetaDate->setMeta (Common::E_AlbumName, QVariant(str));
        }

        str = TStringToQString(tag->comment());
        if (!str.isEmpty ()) {
            targetMetaDate->setMeta (Common::E_Comment, QVariant(str));
        }

        str = TStringToQString(tag->genre());
        if (!str.isEmpty ()) {
            targetMetaDate->setMeta (Common::E_Genre, QVariant(str));
        }

        str = QString::number(tag->year());
        if (!str.isEmpty ()) {
            targetMetaDate->setMeta (Common::E_Year, QVariant(str));
        }
    }

    TagLib::AudioProperties *ap = mTagRef->audioProperties ();

    if (ap) {
        targetMetaDate->setMeta (Common::E_SongLength, QVariant(ap->length ()));

        str = QString::number(ap->sampleRate ());
        if (!str.isEmpty ()) {
            targetMetaDate->setMeta (Common::E_SampleRate, QVariant(str));
        }

        str = QString::number(ap->bitrate());
        if (!str.isEmpty ()) {
            targetMetaDate->setMeta (Common::E_Bitrate, QVariant(str));
        }
    }
    //get cover image
    QImage image = getImage ();
    if (!image.isNull ()) {
        Settings *s = Settings::getInstance ();
        QString imagePath = s->getMusicImageCachePath ();
        QString tmp = name;
        tmp = tmp.mid (0, tmp.lastIndexOf ("."));
        //loop to try to save cover image
        do {
            QString fileName = QString("%1/%2_cover.png").arg (imagePath).arg (tmp);
            if (image.save (fileName, "PNG")) {
                targetMetaDate->setMeta (Common::E_CoverArtSmall, fileName);
                break;
            }
            fileName = QString("%1/%2_cover.bmp").arg (imagePath).arg (tmp);
            if (image.save (fileName, "BMP")) {
                targetMetaDate->setMeta (Common::E_CoverArtSmall, fileName);
                break;
            }
            fileName = QString("%1/%2_cover.jpg").arg (imagePath).arg (tmp);
            if (image.save (fileName, "JPG")) {
                targetMetaDate->setMeta (Common::E_CoverArtSmall, fileName);
                break;
            }
            fileName = QString("%1/%2_cover.jpeg").arg (imagePath).arg (tmp);
            if (image.save (fileName, "JPEG")) {
                targetMetaDate->setMeta (Common::E_CoverArtSmall, fileName);
                break;
            }
            fileName = QString("%1/%2_cover.gif").arg (imagePath).arg (tmp);
            if (image.save (fileName, "GIF")) {
                targetMetaDate->setMeta (Common::E_CoverArtSmall, fileName);
                break;
            }
        } while (true);
    }

    return true;
}

QImage TagParserPro::getImage()
{
    QImage image;

    if (!isValid())
        return image;

    TagLib::File *tagFile = mTagRef->file ();

    if (auto *file = dynamic_cast<TagLib::APE::File *>(tagFile)) {
        if (file->APETag())
            image = fromApe(file->APETag());
    } else if (auto *file = dynamic_cast<TagLib::ASF::File *>(tagFile)) {
        if (file->tag())
            image = fromAsf(file->tag());
    } else if (auto *file = dynamic_cast<TagLib::FLAC::File *>(tagFile)) {
        image = fromFlac(file);

        if (image.isNull() && file->ID3v2Tag())
            image = fromId3(file->ID3v2Tag());
    } else if (auto *file = dynamic_cast<TagLib::MP4::File *>(tagFile)) {
        if (file->tag())
            image = fromMp4(file->tag());
    } else if (auto *file = dynamic_cast<TagLib::MPC::File *>(tagFile)) {
        if (file->APETag())
            image = fromApe(file->APETag());
    } else if (auto *file = dynamic_cast<TagLib::MPEG::File *>(tagFile)) {
        if (file->ID3v2Tag())
            image = fromId3(file->ID3v2Tag());

        if (image.isNull() && file->APETag())
            image = fromApe(file->APETag());
    } else if (auto *file = dynamic_cast<TagLib::Ogg::Vorbis::File *>(tagFile)) {
        if (file->tag())
            image = fromVorbis(file->tag());
    } else if (auto *file = dynamic_cast<TagLib::WavPack::File *>(tagFile)) {
        if (file->APETag())
            image = fromApe(file->APETag());
    }

    return image;
}

bool TagParserPro::isValid()
{
    return (mTagRef != nullptr)
            && mTagRef->file () && mTagRef->file ()->isValid ();
}

QImage TagParserPro::fromTagBytes(const TagLib::ByteVector &data)
{
    QImage image;
    image.loadFromData((const uchar *)data.data(), data.size());
    return image;
}

QImage TagParserPro::fromApe(TagLib::APE::Tag *tag)
{
    const TagLib::APE::ItemListMap &map = tag->itemListMap();

    TagLib::String str = "COVER ART (FRONT)";
    if (!map.contains(str))
        return QImage();

    TagLib::String fileName = map[str].toString();
    TagLib::ByteVector item = map[str].value();
    return fromTagBytes(item.mid(fileName.size() + 1));
}

QImage TagParserPro::fromAsf(TagLib::ASF::Tag *tag)
{
    const TagLib::ASF::AttributeListMap &map = tag->attributeListMap();

    TagLib::String str = "WM/Picture";
    if (!map.contains(str))
        return QImage();

    const TagLib::ASF::AttributeList &list = map[str];
    if (list.isEmpty())
        return QImage();

    TagLib::ASF::Picture pic = list[0].toPicture();
    if (pic.isValid())
        return fromTagBytes(pic.picture());

    return QImage();
}

QImage TagParserPro::fromFlac(TagLib::FLAC::File *file)
{
    const TagLib::List<TagLib::FLAC::Picture *> &list = file->pictureList();
    if (list.isEmpty())
        return QImage();

    TagLib::FLAC::Picture *pic = list[0];
    return fromTagBytes(pic->data());
}

QImage TagParserPro::fromId3(TagLib::ID3v2::Tag *tag)
{
    const TagLib::ID3v2::FrameList &list = tag->frameList("APIC");
    if (list.isEmpty())
        return QImage();

    auto *frame = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(list.front());
    return fromTagBytes(frame->picture());
}

QImage TagParserPro::fromMp4(TagLib::MP4::Tag *tag)
{
    TagLib::String str = "covr";
    if (!tag->itemListMap().contains(str))
        return QImage();

    TagLib::MP4::CoverArtList coverList = tag->itemListMap()[str].toCoverArtList();
    if (coverList[0].data().size() > 0)
        return fromTagBytes(coverList[0].data());

    return QImage();
}

QImage TagParserPro::fromVorbis(TagLib::Tag *tag)
{
    if (auto *comment = dynamic_cast<TagLib::Ogg::XiphComment *>(tag)) {
        TagLib::String str = "COVERART";

        if (!comment->contains(str))
            str = "METADATA_BLOCK_PICTURE";

        if (!comment->contains(str))
            return QImage();

        TagLib::ByteVector tagBytes = comment
                ->fieldListMap()[str].front().data(TagLib::String::Latin1);
        QByteArray base64;
        base64.setRawData(tagBytes.data(), tagBytes.size());
        QImage image;
        image.loadFromData(QByteArray::fromBase64(base64));
        return image;
    }

    return QImage();
}

} //TagParserPro
} //MusicLibrary
} //PhoenixPlayer
