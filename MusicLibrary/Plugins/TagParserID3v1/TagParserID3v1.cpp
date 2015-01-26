
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QTextCodec>

#include "TagParserID3v1.h"
#include "SongMetaData.h"
#include "Util.h"

namespace PhoenixPlayer {
namespace MusicLibrary {
namespace TagParserID3v1 {


TagParserID3v1::TagParserID3v1(QObject *parent)
    : IMusicTagParser(parent)
{
    mDefaultCodec = Util::localeDefaultCodec ();
}

TagParserID3v1::~TagParserID3v1()
{
}

bool TagParserID3v1::parserTag(SongMetaData *targetMetaDate)
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

    //Check is the file size is enough.
    int fileSize = musicFile.size ();
    //ID3v1 is 128 bytes, so if the file size is less than 128, it can't have ID3v1.
    if (fileSize < 128)
        return false;

    //Create the tag caches.
    char rawTagData[128];
    ID3v1Struct tagData;

    //ID3V1 tag is in end of the file
    //Read the raw tag data.
    musicDataStream.skipRawData(fileSize-128);
    musicDataStream.readRawData(rawTagData, 128);
    //Check is the header 'TAG':
    if (rawTagData[0]!='T' || rawTagData[1]!='A' || rawTagData[2]!='G')
        return false;

    //Parse the raw data.
    parseRawData(rawTagData, tagData);
    //Write raw data to the detail info.
    //Name
    targetMetaDate->setSongTitle (tagData.tags[0]);
    //Artist
    targetMetaDate->setArtistName (tagData.tags[1]);
    //Album
    targetMetaDate->setAlbumName (tagData.tags[2]);
    //Year
    targetMetaDate->setYear (tagData.tags[3].toInt ());
    //Comments
    targetMetaDate->setSongDescription (tagData.tags[4]);
    //TODO : need to add Genre
    return true;
}

void TagParserID3v1::parseRawData(char *rawTagData, ID3v1Struct &tagData)
{
    char lastBackupData;
    int lastBackupPosition=-1;
    //ID3v1 is very simple, its data is stored in order:
    //  30 bytes Title + 30 bytes Artist + 30 bytes Album + 4 bytes Year +
    //  30 bytes Comments(+Track) + 1 bytes Genre
    //Just read them by order.
    backupByte(rawTagData, 33, lastBackupData, lastBackupPosition, true);
    //Title
    tagData.tags[0] = standardizeText(mDefaultCodec->toUnicode(rawTagData+3));
    backupByte(rawTagData, 63, lastBackupData, lastBackupPosition, true);
    //Artist
    tagData.tags[1] = standardizeText(mDefaultCodec->toUnicode(rawTagData+33, 30));
    backupByte(rawTagData, 93, lastBackupData, lastBackupPosition, true);
    //Album
    tagData.tags[2] = standardizeText(mDefaultCodec->toUnicode(rawTagData+63, 30));
    backupByte(rawTagData, 97, lastBackupData, lastBackupPosition, true);
    //Year
    tagData.tags[3] = standardizeText(mDefaultCodec->toUnicode(rawTagData+93, 4));
    backupByte(rawTagData, 127, lastBackupData, lastBackupPosition, true);
    //Comment is a little complex: check the No.125 char first, if it's 0, then
    //the following char is track index.
    if(rawTagData[125] == 0) {
        tagData.track = (quint8)rawTagData[126];
    }
    tagData.tags[4] = standardizeText(mDefaultCodec->toUnicode(rawTagData+97));
    //Genre index.
    tagData.genreIndex = lastBackupData;

}

QString TagParserID3v1::standardizeText(const QString &text)
{
    return text.simplified ().remove (QChar('\0'));
}

void TagParserID3v1::backupByte(char *rawTagData, const int &backupPosition, char &backupPool, int &positionPool, const bool &clearData)
{
    //Check the position backup pool, if it's not null(-1), restore the data.
    if (positionPool != -1) {
        rawTagData[positionPool] = backupPool;
    }
    //Do the backup.
    backupPool = rawTagData[backupPosition];
    positionPool = backupPosition;
    //If ask to clear the data, clear it.
    if(clearData) {
        rawTagData[backupPosition] = 0;
    }
}

} //TagParserID3v1
} //MusicLibrary
} //PhoenixPlayer
