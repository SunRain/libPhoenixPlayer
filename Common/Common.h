#ifndef COMMON_H
#define COMMON_H
#include <QObject>
namespace PhoenixPlayer {
class Common
{
    Q_GADGET
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
};
}
#endif // COMMON_H
