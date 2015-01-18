#ifndef COMMON_H
#define COMMON_H
#include <QObject>
#include <QMetaEnum>

namespace PhoenixPlayer {

class Common : public QObject
{
    Q_OBJECT
    Q_ENUMS(PlaybackState)
    Q_ENUMS(PlayMode)
    Q_ENUMS(MusicLibraryElement)
public:
    enum PlaybackState {
        PlaybackStopped = 0x0,  //播放停止
        PlaybackPlaying,        //播放
        PlaybackPaused          //暂停
    };
    enum PlayMode {
        ModeOrder = 0x0,            //顺序播放
        ModeRepeatAll,              //循环播放
        ModeRepeatCurrent,          //单曲循环
        ModeShuffle                 //随机播放
    };
    enum MediaType {
        TypeLocalFile = 0x0,        //本地文件
        TypeUrl,                    //网络文件
        TypeStream                  //媒体文件
    };

    enum MusicLibraryElement {
        E_NULLElement = 0x0,
        E_PlayListName,        //播放列表名称
        E_Hash,
        E_FilePath,
        E_FileName,
        E_MediaBitrate,
        E_FileSize,
        E_ArtistName,
        E_ArtistImageUri,
        E_ArtistDescription,
        E_AlbumName,
        E_AlbumImageUrl,
        E_AlbumDescription,
        E_AlbumYear,
        E_CoverArtSmall,
        E_CoverArtLarge,
        E_CoverArtMiddle,
        E_MediaType,
        E_SongLength,
        E_SongTitle,
        E_SongDescription,
        E_Category,
        E_Year,
        E_Date,
        E_UserRating,
        E_Keywords,
        E_Language,
        E_Publisher,
        E_Copyright,
        E_Lyrics,
        E_Mood
    };

    QString enumToStr(const QString &enumName, int enumValue)
    {
        int index  = metaObject ()->indexOfEnumerator (enumName.toLocal8Bit ());
        QMetaEnum m = metaObject ()->enumerator (index);
        return m.valueToKey (enumValue);
    }
};
}
#endif // COMMON_H
