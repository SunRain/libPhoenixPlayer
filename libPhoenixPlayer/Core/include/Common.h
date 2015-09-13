#ifndef COMMON_H
#define COMMON_H
#include <QObject>
#include <QMetaEnum>


#include "BaseObject.h"

namespace PhoenixPlayer {

class Common : public BaseObject
{
    Q_OBJECT
    Q_ENUMS(PlayBackendState)
    Q_ENUMS(PlayMode)
    Q_ENUMS(SongMetaTags)
    Q_ENUMS(PlayListElement)
    Q_ENUMS(PluginType)
public:
    enum PlayBackendState {
        PlayBackendStopped = 0x0,  //播放停止
        PlayBackendPlaying,        //播放
        PlayBackendPaused          //暂停
    };
    enum PlayMode {
        PlayModeOrder = 0x10,            //顺序播放
        PlayModeRepeatAll,              //循环播放
        PlayModeRepeatCurrent,          //单曲循环
        PlayModeShuffle                 //随机播放
    };
    enum MediaType {
        MediaTypeLocalFile = 0x20,        //本地文件
        MediaTypeUrl,                    //网络文件
        MediaTypeStream                  //媒体文件
    };

    enum PlayListElement {
        PlayListFirstFlag = 0x30,
        PlayListHash,
        PlayListName,
        PlayListSongHashes,
        PlayListLastFlag
    };

    enum SongMetaTags {
        E_FirstFlag = 0x40,
        E_Hash, //查询数据时候的主键,后续数据库查询语句依赖于枚举序列顺序,故必须处于枚举序列的第二位
        E_FilePath,
        E_FileName,
        E_Bitrate,
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
        E_Mood,
        E_Composer,
        E_Conductor	,
        E_Genre,
        E_Comment,
        E_SampleRate,
        E_LastFlag
    };

    //系统插件
    enum PluginType {
        PluginTypeAll = 0x100,
        //为保证后续兼容,TypePlayBackend必须为第二个项
        PluginPlayBackend,              //播放后端
        PluginPlayListDAO,              //音乐库存储后端
        PluginMusicTagParser,           //音乐Tag解析
        PluginMetadataLookup,           //metadata查询
        PluginDecoder,                  //解码插件
        PluginOutPut,                   //输出插件
        PluginTypeUndefined             //最后一个标记,为保证兼容,所有后续添加的枚举必须在此项之前
    };

//    QString enumToStr(const QString &enumName, int enumValue)
//    {
//        int index  = metaObject ()->indexOfEnumerator (enumName.toLocal8Bit ());
//        QMetaEnum m = metaObject ()->enumerator (index);
//        return m.valueToKey (enumValue);
//    }
};
}
#endif // COMMON_H
