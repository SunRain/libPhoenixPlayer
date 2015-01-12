#ifndef METADATA_H
#define METADATA_H

#include <QString>

namespace PhoenixPlayerCore {

///
/// \brief 作者信息
///
struct Artist {
    QString name; //作者姓名
    QString imageUrl; //图片
    QString description; //介绍
};

///
/// \brief 唱片信息
///
struct Album {
    QString name;
    QString imageUrl;
    QString description; //介绍
    QString year; //唱片发型日期
//    Artist artist;
};

///
/// \brief The SongType enum
///
enum SongType{
    SongLocalFile = 0x1,
//    SongRadio,
    SongInternet
};

///
/// \brief The SongMetaDate struct
///
struct SongMetaDate {
    int id;
    Artist artist;
    Album album;
    SongType type;
    int length;
    int bitrate;
    int fileSize;

    QString filePath;
    QString fileName;
    QString songTitle;
    QString comment;

    bool playSelected;
    bool isPlaying;

//    qint32 id;
//    qint32 album_id;
//    qint32 artist_id;
//    QString title;
//    QString artist;
//    QString album;
//    QStringList genres;
//    qint32 rating;
//    qint64 length_ms;
//    qint32 year;
//    QString filepath;
//    qint32 track_num;
//    qint32 bitrate;
//    qint64 filesize;
//    QString comment;
//    int discnumber;
//    int n_discs;


//    bool is_extern;
//    int radio_mode;



//    bool pl_selected;
//    bool pl_playing;
//    bool pl_dragged;

//    bool is_lib_selected;
//    bool is_disabled;
};
}
#endif // METADATA_H
