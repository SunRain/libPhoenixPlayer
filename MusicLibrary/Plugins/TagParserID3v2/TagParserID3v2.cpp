#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QTextCodec>
#include <QImage>
#include <QLinkedList>

#include "TagParserID3v2.h"
#include "Util.h"
#include "SongMetaData.h"
#include "Settings.h"

namespace PhoenixPlayer {
namespace MusicLibrary {
namespace TagParserID3v2 {

TagParserID3v2::TagParserID3v2(QObject *parent)
    : IMusicTagParser(parent)
{
    //Initial the codec.
    m_isoCodec = QTextCodec::codecForName("ISO 8859-1");
    m_utf16Codec = QTextCodec::codecForName("UTF-16");
    m_utf16BECodec = QTextCodec::codecForName("UTF-16BE");
    m_utf16LECodec = QTextCodec::codecForName("UTF-16LE");
    m_utf8Codec = QTextCodec::codecForName("UTF-8");
    m_localeCodec = Util::localeDefaultCodec ();

    //    //Initial music global.
    //    m_musicGlobal=KNMusicGlobal::instance();

    mSettings = Settings::getInstance ();

    //Initial unsynchronisation data.
    //Using forced conversation to ignore the ambiguous calling.
    mUnSyncRaw.append((char)0xff);
    mUnSyncRaw.append((char)0x00);
    mUnSyncTo.append((char)0xff);

    //Initial frame ID Index.
    mFrameIDIndex["TIT2"] = Name;
    mFrameIDIndex["TPE1"] = Artist;
    mFrameIDIndex["TALB"] = Album;
    mFrameIDIndex["TPE2"] = AlbumArtist;
    mFrameIDIndex["TBPM"] = BeatsPerMinuate;
    mFrameIDIndex["TIT1"] = Category;
    mFrameIDIndex["COMM"] = Comments;
    mFrameIDIndex["TCOM"] = Composer;
    mFrameIDIndex["TIT3"] = Description;
    mFrameIDIndex["TCON"] = Genre;
    mFrameIDIndex["TRCK"] = TrackNumber;
    mFrameIDIndex["TPOS"] = DiscNumber;
    mFrameIDIndex["POPM"] = Rating;
    mFrameIDIndex["TYER"] = Year;

    mFrameIDIndex["TT2"] = Name;
    mFrameIDIndex["TP1"] = Artist;
    mFrameIDIndex["TAL"] = Album;
    mFrameIDIndex["TP2"] = AlbumArtist;
    mFrameIDIndex["TBP"] = BeatsPerMinuate;
    mFrameIDIndex["TT1"] = Category;
    mFrameIDIndex["COM"] = Comments;
    mFrameIDIndex["TCM"] = Composer;
    mFrameIDIndex["TT3"] = Description;
    mFrameIDIndex["TCO"] = Genre;
    mFrameIDIndex["TRK"] = TrackNumber;
    mFrameIDIndex["TPA"] = DiscNumber;
    mFrameIDIndex["POP"] = Rating;
    mFrameIDIndex["TYE"] = Year;
}

TagParserID3v2::~TagParserID3v2()
{

}

bool TagParserID3v2::parserTag(SongMetaData *targetMetaDate)
{
    QString f = QString("%1/%2")
            .arg (targetMetaDate->filePath ())
            .arg (targetMetaDate->fileName ());
    QFile musicFile(f);
    if (!musicFile.open (QIODevice::ReadOnly)) {
        qDebug()<<"Can't open music file "<< f;
        return false;
    }

    //Initial a binary data stream for music file reading.
    QDataStream musicDataStream (&musicFile);

    //If file is less than ID3v2 header, it can't contains ID3v2 tag.
    if (musicFile.size () < 10) {
        return false;
    }
    //Initial datas.
    char rawHeader[10];
    ID3v2Header header;
    //Detect ID3v2 header.
    musicDataStream.readRawData (rawHeader, 10);
    if (!parseID3v2Header(rawHeader, header)) {
        return false;
    }

    //Check is file's size smaller than tag size.
    if (musicFile.size() < (header.size+10)) {
        //File is smaller than the tag says, failed to get.
        return false;
    }
    //Read the raw tag data.
    char *rawTagData = new char[header.size];
    musicDataStream.readRawData (rawTagData, header.size);
    //Parse these raw data.
    QLinkedList<ID3v2Frame> frames;
    ID3v2MinorProperty property;
    generateID3v2Property (header.minor, property);
    parseID3v2RawData (rawTagData, header, property, frames);
    //Write the tag to details.
    if (!frames.isEmpty()) {
        writeID3v2ToDetails(frames, property, targetMetaDate);
    }
    //Recover the memory.
    delete[] rawTagData;
    if (parseAlbumArt ()) {
        QString imagePath = mSettings->getMusicImageCachePath ();
        QImage image = mID3v2Item.coverImage;
        QString fileName = targetMetaDate->fileName ();
        fileName = fileName.mid (0, fileName.lastIndexOf ("."));
        fileName = QString("%1/%2_cover").arg (imagePath).arg (fileName);
        image.save (fileName);
        targetMetaDate->setCoverArtSmall (fileName);



    }
    return true;
}

bool TagParserID3v2::parseAlbumArt()
{
    if (!mID3v2Item.imageData.contains("ID3v2")) {
        return false;
    }
    //Get the total size of images.
    QByteArray imageTypes = mID3v2Item.imageData["ID3v2"].takeLast();
    int imageCount = imageTypes.size();
    QHash<int, ID3v2PictureFrame> imageMap;
    for (int i=0; i<imageCount; ++i) {
        //Check the flag is "APIC" or "PIC"
        if (imageTypes.at(i) == 1) {
            parseAPICImageData(mID3v2Item.imageData["ID3v2_Images"].takeFirst(),
                    imageMap);
        } else {
            parsePICImageData(mID3v2Item.imageData["ID3v2_Images"].takeFirst(),
                    imageMap);
        }
    }
    //If there's a album art image after parse all the album art, set.
    if(imageMap.contains(3)) {
        mID3v2Item.coverImage = imageMap[3].image;
    } else {
        //Or else use the first image.
        if(!imageMap.isEmpty()) {
            mID3v2Item.coverImage = imageMap.begin().value().image;
        }
    }
    return true;
}

QString TagParserID3v2::frameToText(QByteArray content)
{
    //Check is content empty.
    if (content.isEmpty ()) {
        return QString();
    }
    //Get the codec according to the first char.
    //The first char of the ID3v2 text is the encoding of the current text.
    //Using a state to catch the convert state.
    quint8 encoding = (quint8)(content.at (0));
    content.remove (0, 1);
    switch(encoding) {
    case EncodeISO: //0 = ISO-8859-1
        //Use unicode codec to translate.
        return mUsingDefaultCodec
                ? m_localeCodec->toUnicode (content).simplified ().remove (QChar('\0'))
                : m_isoCodec->toUnicode (content).simplified ().remove (QChar('\0'));
    case EncodeUTF16BELE: //1 = UTF-16 LE/BE (Treat other as no BOM UTF-16)
        //Decode via first two bytes.
        if (content.size ()> 1){
            if((quint8)content.at(0) == 0xFE && (quint8)content.at(1) == 0xFF) {
                return m_utf16BECodec->toUnicode(content).simplified().remove(QChar('\0'));
            }
            if((quint8)content.at(0) == 0xFF && (quint8)content.at(1) == 0xFE) {
                return m_utf16LECodec->toUnicode(content).simplified().remove(QChar('\0'));
            }
        }
        return m_utf16Codec->toUnicode(content).simplified().remove(QChar('\0'));
    case EncodeUTF16: //2 = UTF-16 BE without BOM
        //Decode with UTF-16
        return m_utf16Codec->toUnicode(content).simplified().remove(QChar('\0'));
    case EncodeUTF8: //3 = UTF-8
        //Use UTF-8 to decode it.
        return m_utf8Codec->toUnicode(content).simplified().remove(QChar('\0'));
    default://Use locale codec.
        return m_localeCodec->toUnicode(content).simplified().remove(QChar('\0'));
    }
}

bool TagParserID3v2::usingDefaultCodec() const
{
    return mUsingDefaultCodec;
}

void TagParserID3v2::setUsingDefaultCodec(bool usingDefaultCodec)
{
    mUsingDefaultCodec = usingDefaultCodec;
}

bool TagParserID3v2::parseID3v2Header(char *rawHeader, ID3v2Header &header)
{
    //Check 'ID3' from the very beginning.
    if (rawHeader[0] != 'I' || rawHeader[1] != 'D' || rawHeader[2] != '3') {
        return false;
    }
    //Get the version of the tag.
    header.major = (quint8)rawHeader[3];
    header.minor = (quint8)rawHeader[3];
    //Get the flag.
    header.flag = (quint8)rawHeader[5];
    //Calculate tag size.
    header.size = (((quint32)rawHeader[6]<<21)&0b00001111111000000000000000000000)+
            (((quint32)rawHeader[7]<<14)&0b00000000000111111100000000000000)+
            (((quint32)rawHeader[8]<<7) &0b00000000000000000011111110000000)+
            ( (quint32)rawHeader[9]     &0b00000000000000000000000001111111);
    return true;
}

void TagParserID3v2::generateID3v2Property(const quint8 &minor, ID3v2MinorProperty &property)
{
    //Because the ID3v2 has so many version, we have to use different calculate
    //function to process these frames.
    switch(minor) {
    case 0:
    case 1:
    case 2:
        property.frameIDSize = 3;
        property.frameHeaderSize = 6;
        property.toSize = TagParserID3v2::minor2Size;
        property.saveFlag = nullptr;
        break;
    case 3:
        property.frameIDSize = 4;
        property.frameHeaderSize = 10;
        property.toSize = TagParserID3v2::minor3Size;
        property.saveFlag = TagParserID3v2::saveFlag;
        break;
    case 4:
        property.frameIDSize = 4;
        property.frameHeaderSize = 10;
        property.toSize = TagParserID3v2::minor4Size;
        property.saveFlag = TagParserID3v2::saveFlag;
        break;
    default:
        break;
    }
}

bool TagParserID3v2::parseID3v2RawData(char *rawTagData, const ID3v2Header &header, const ID3v2MinorProperty &property, QLinkedList<ID3v2Frame> &frameList)
{
    char *rawPosition = rawTagData;
    quint32 rawTagDataSurplus = header.size;
    while (rawTagDataSurplus > 0) {
        //If no tags, means behind of these datas are all '\0'.
        if (rawPosition[0] == 0) {
            break;
        }
        //Calculate the size first.
        quint32 frameSize = ((*(property.toSize))(rawPosition+property.frameIDSize));
        //Check the frame size.
        if(frameSize <= 0 || frameSize > rawTagDataSurplus) {
            break;
        }
        ID3v2Frame currentFrame;
        //Set the frame ID.
        strncpy(currentFrame.frameID, rawPosition, property.frameIDSize);
        //Set the start position and size.
        currentFrame.start = rawPosition+property.frameHeaderSize;
        currentFrame.size = frameSize;
        //Process the flag.
        if(property.saveFlag != nullptr) {
            (*(property.saveFlag))(rawPosition, currentFrame);
        }
        //Append the frame to the list.
        frameList.append(currentFrame);
        //Reduce the surplus, jump to the next position.
        int frameContentSize = property.frameHeaderSize+currentFrame.size;
        rawTagDataSurplus -= frameContentSize;
        rawPosition = rawPosition + frameContentSize;
    }
    return true;
}

void TagParserID3v2::writeID3v2ToDetails(const QLinkedList<ID3v2Frame> &frames,
                                         const ID3v2MinorProperty &property,
                                         SongMetaData *data)
{
    //Get the detail info.
    //KNMusicDetailInfo &detailInfo = data.detailInfo;
    mID3v2Item.data = data;

    //Prepare the image type list.
    QByteArray imageTypeList;
//    for (QLinkedList<ID3v2Frame>::const_iterator i = frames.begin();
//         i != frames.end();
//         ++i) {
    foreach (ID3v2Frame i, frames) {
        //Process the data according to the flag before we use it.
        QByteArray frameData;
        //Check if it contains a data length indicator.
        if (i.flags[1] & FrameDataLengthIndicator) {
            frameData = QByteArray(i.start + 4,
                                 ((*(property.toSize))(i.start)));
        } else {
            frameData = QByteArray(i.start, i.size);
        }
        //Check if the frame is unsynchronisation.
        if(i.flags[1] & FrameUnsynchronisation) {
            frameData.replace(mUnSyncRaw,
                              mUnSyncTo);
        }
        //Get the frame Index.
        QString frameID = QString(i.frameID).toUpper();
        if (frameID == "APIC" || frameID == "PIC") {
            //Here is a hack:
            //Using "ID3v2" as a counter, the size of the byte array is the
            //number of how many images contains in ID3v2.
            //If the frameID is "APIC", add a 1 to the "ID3v2" array, or else
            //add a 0.
            imageTypeList.append ((int)(frameID == "APIC"));
            //data.imageData["ID3v2_Images"].append(frameData);
            mID3v2Item.imageData["ID3v2_Images"].append(frameData);
            continue;
        }
        if (!mFrameIDIndex.contains(i.frameID)) {
            continue;
        }

        int frameIndex = mFrameIDIndex[i.frameID];
//        int diagonalPosition;
//        int rightBracketsPosition;
//        QString frameText;

        switch (frameIndex) {
        case Name:
            mID3v2Item.data->setSongTitle (frameToText (frameData));
            break;
        case Album:
            mID3v2Item.data->setAlbumName (frameToText (frameData));
            break;
        case AlbumArtist:
            mID3v2Item.data->setArtistName (frameToText (frameData));
            break;
        case AlbumRating:
            break;
        case Artist:
            mID3v2Item.data->setArtistName (frameToText (frameData));
            break;
        case BeatsPerMinuate:
            break;
        case BitRate:
            mID3v2Item.data->setMediaBitrate (frameToText (frameData).toInt ());
            break;
        case Category:
            mID3v2Item.data->setCategory (QStringList((frameToText (frameData))));
            break;
        case Description:
            mID3v2Item.data->setSongDescription (frameToText (frameData));
            break;
        case Size:
            //TODO 是否是指实际的文件大小?
            qDebug()<<"============== size " + (frameToText (frameData));
            break;
        case Time:
            mID3v2Item.data->setSongLength (frameToText (frameData).toInt ());
            break;
        case Year:
            mID3v2Item.data->setYear (QString(frameData.at(0)).toInt ());
            break;
        case DiscNumber:
            //TODO: Don't need for our struct
//            //Get the text of the frame.
//            frameText = frameToText(frameData);
//            //Search the character '/', check whether the format is like 1/9
//            diagonalPosition = frameText.indexOf('/');
//            //If there's no diagonal, means it only contains disc number.
//            if(diagonalPosition == -1) {
//                setTextData(detailInfo.textLists[DiscNumber], frameText);
//            } else {
//                setTextData(detailInfo.textLists[DiscNumber],
//                            frameText.left(diagonalPosition));
//                setTextData(detailInfo.textLists[DiscCount],
//                            frameText.mid(diagonalPosition+1));
//            }
            break;
        case TrackNumber:
            //TODO: Don't need for our struct
//            //Get the text of the frame.
//            frameText = frameToText(frameData);
//            //Search the character '/', check whether the format is like 1/9
//            diagonalPosition = frameText.indexOf('/');
//            //If there's no diagonal, means it only contains track number.
//            if(diagonalPosition == -1) {
//                setTextData(detailInfo.textLists[TrackNumber], frameText);
//            } else {
//                setTextData(detailInfo.textLists[TrackNumber], frameText.left(diagonalPosition));
//                setTextData(detailInfo.textLists[TrackCount], frameText.mid(diagonalPosition+1));
//            }
            break;
        case Genre:
            //TODO We don't have Genre  atm
//            //Get the text of the frame.
//            frameText = frameToText(frameData);
//            //Check is the frame text is format like: (9) or (9) XXX
//            if(frameText[0] == '(') {
//                //Check is there any ')'.
//                rightBracketsPosition = frameText.indexOf(')');
//                if(rightBracketsPosition == -1) {
//                    //This text is like '(XXXXX', WTF, remove first '('.
//                    frameText.remove(0, 1);
//                    bool indexTranslateOk = false;
//                    //Try to translate this into a number.
//                    int attemptIndex = frameText.toInt(&indexTranslateOk);
//                    //If it's a number, set the indexed genre, or set the raw text.
//                    setTextData(detailInfo.textLists[Genre],
//                                indexTranslateOk?
//                                    m_musicGlobal->indexedGenre(attemptIndex):
//                                    frameText);
//                } else {
//                    //Try to translate the content in the brackets.
//                    bool indexTranslateOk = false;
//                    int attemptIndex = frameText.mid(1, rightBracketsPosition-1).toInt(&indexTranslateOk);
//                    //If it's a number, set the indexed genre, or set the raw text.
//                    setTextData(detailInfo.textLists[Genre],
//                                indexTranslateOk?
//                                    m_musicGlobal->indexedGenre(attemptIndex):
//                                    frameText);
//                }
//            } else {
//                //Treat the whole text as genre.
//                setTextData(detailInfo.textLists[Genre],
//                            frameText);
//            }
            break;
        case Rating:
            //I don't know why all the file using this.
            if (QString(frameData.left(29)) == "Windows Media Player 9 Series") {
                //Translate the last bytes as rating.
                mID3v2Item.data->setUserRating (ratingStars((quint8)(frameData.at(30))));
            } else {
                //Treat the first bytes as rating.
                if(!frameData.isEmpty()) {
                    mID3v2Item.data->setUserRating (QString(frameData.at(0)).toInt ());
                }
            }
            break;
        case Comments:
//            mID3v2Item.data->setSongDescription (frameToText (frameData));
            break;
        case Composer:
        case DateAdded:
        case DateModified:
        case DiscCount:
        case Kind:
        case LastPlayed:
        case Plays:
        case SampleRate:
        case TrackCount:
        case MusicDataCount:
        default:
//            setTextData(detailInfo.textLists[frameIndex],
//                        frameToText(frameData));
            break;
        }
    }
    //If there's any data in type list, add it to image data.
    if (!imageTypeList.isEmpty()) {
        mID3v2Item.imageData["ID3v2"].append(imageTypeList);
    }
}

int TagParserID3v2::ratingStars(const quint8 &hex)
{
    //1-31  = 1 star.
    //32-95 = 2 stars.
    //96-159 = 3 stars.
    //160-223 = 4 stars.
    //224-255 = 5 stars.
    if(hex>0 && hex<32) {
        return 1;
    }
    if(hex>31 && hex<96) {
        return 2;
    }
    if(hex>95 && hex<160) {
        return 3;
    }
    if(hex>159 && hex<224) {
        return 4;
    }
    if(hex>223) {
        return 5;
    }
    return 0;
}

quint32 TagParserID3v2::minor2Size(char *rawTagData)
{
    return (((quint32)rawTagData[0]<<16)&0b00000000111111110000000000000000)+
            (((quint32)rawTagData[1]<<8) &0b00000000000000001111111100000000)+
            ( (quint32)rawTagData[2]     &0b00000000000000000000000011111111);
}

quint32 TagParserID3v2::minor3Size(char *rawTagData)
{
    return (((quint32)rawTagData[0]<<24)&0b11111111000000000000000000000000)+
            (((quint32)rawTagData[1]<<16)&0b00000000111111110000000000000000)+
            (((quint32)rawTagData[2]<<8 )&0b00000000000000001111111100000000)+
            ( (quint32)rawTagData[3]     &0b00000000000000000000000011111111);
}

quint32 TagParserID3v2::minor4Size(char *rawTagData)
{
    return (((quint32)rawTagData[0]<<21)&0b00001111111000000000000000000000)+
            (((quint32)rawTagData[1]<<14)&0b00000000000111111100000000000000)+
            (((quint32)rawTagData[2]<<7 )&0b00000000000000000011111110000000)+
            ( (quint32)rawTagData[3]     &0b00000000000000000000000001111111);
}

void TagParserID3v2::saveFlag(char *rawTagData, ID3v2Frame &frameData)
{
    frameData.flags[0] = rawTagData[8];
    frameData.flags[1] = rawTagData[9];
}

void TagParserID3v2::parseAPICImageData(QByteArray imageData,
                                        QHash<int, ID3v2PictureFrame> &imageMap)
{
    //APIC contains:
    /*
          Text encoding      $xx
          MIME type          <text string> $00
          Picture type       $xx
          Description        <text string according to encoding> $00 (00)
          Picture data       <binary data>
        */
    //In the official document above, (00) in the description means there's a
    //00 byte after the $00, I finally understand what this mean.
    ID3v2PictureFrame currentFrame;
    //Get mime end and description end.
    int mimeTypeEnd = imageData.indexOf('\0', 1);
    int descriptionEnd = mimeTypeEnd + 2;
    //Backup the text encording and get the picture type.
    quint8 textEncoding = imageData.at(0);
    quint8 pictureType = imageData.at(mimeTypeEnd+1);
    switch (textEncoding) {
    case EncodeISO:
    case EncodeUTF8:
        descriptionEnd = imageData.indexOf('\0', mimeTypeEnd+2);
        break;
    case EncodeUTF16BELE:
    case EncodeUTF16:
        while (imageData.at(descriptionEnd) != '\0'
               || imageData.at(descriptionEnd+1) != '\0') {
            descriptionEnd += 2;
        }
        break;
    default:
        break;
    }
    //Get the mime type text.
    currentFrame.mimeType = frameToText(imageData.left(mimeTypeEnd));
    //Get the description text.
    QByteArray descriptionText = imageData.mid(mimeTypeEnd+2,
                                               descriptionEnd-(mimeTypeEnd+2));
    descriptionText.insert(0, textEncoding);
    currentFrame.description = frameToText(descriptionText);
    //Get the image.
    switch (textEncoding) {
    case EncodeISO:
    case EncodeUTF8:
        imageData.remove (0, descriptionEnd+1);
        break;
    case EncodeUTF16BELE:
    case EncodeUTF16:
        imageData.remove (0, descriptionEnd+2);
        break;
    default:
        break;
    }
    currentFrame.image.loadFromData(imageData);
    //If parse the image success, add it to map.
    if (!currentFrame.image.isNull()) {
        imageMap[pictureType] = currentFrame;
    }
}

void TagParserID3v2::parsePICImageData(QByteArray imageData,
                                       QHash<int, ID3v2PictureFrame> &imageMap)
{
    //PIC contains:
    /*
         Text encoding      $xx
         Image format       $xx xx xx
         Picture type       $xx
         Description        <textstring> $00 (00)
         Picture data       <binary data>
       */
    //In the official document above, the (00) in description is the same as APIC.
    ID3v2PictureFrame currentFrame;
    //Get the mime type text.
    currentFrame.mimeType = imageData.mid(1, 3);
    //Backup the text encording and get the picture type.
    quint8 textEncoding = imageData.at(0);
    quint8 pictureType = imageData.at(4);
    //Get description end.
    int descriptionEnd = 5;
    switch (textEncoding) {
    case EncodeISO:
    case EncodeUTF8:
        descriptionEnd = imageData.indexOf('\0', 5);
        break;
    case EncodeUTF16BELE:
    case EncodeUTF16:
        while(imageData.at(descriptionEnd) != '\0'
              || imageData.at(descriptionEnd+1) != '\0') {
            descriptionEnd += 2;
        }
        break;
    default:
        break;
    }
    //Get the description.
    QByteArray descriptionText = imageData.mid(5, descriptionEnd-5);
    descriptionText.append(textEncoding);
    currentFrame.description = frameToText(descriptionText);
    //Get the image.
    switch (textEncoding) {
    case EncodeISO:
    case EncodeUTF8:
        imageData.remove(0, descriptionEnd+1);
        break;
    case EncodeUTF16BELE:
    case EncodeUTF16:
        imageData.remove(0, descriptionEnd+2);
        break;
    default:
        break;
    }
    currentFrame.image.loadFromData(imageData);
    //If parse the image success, add it to map.
    if(!currentFrame.image.isNull()) {
        imageMap[pictureType] = currentFrame;
    }
}

} //TagParserID3v2
} //MusicLibrary
} //PhoenixPlayer
