#ifndef PHOENIXBACKEND_GLOBAL_H
#define PHOENIXBACKEND_GLOBAL_H

#include <QString>

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

//TODO change this to a  c++ class ?

//#define BUFFER_MSEC 1000 //buffer size in milliseconds

enum PlayState
{
    Playing = 0x1000,   //The player is playing source
    Paused,             //The player has currently paused its playback
    Stopped,            //The player is ready to play source
    Buffering,          //The Player is waiting for data to be able to start playing.
    NormalError,        //Input source is invalid or unsupported. Player should skip this file
    FatalError          //This means unrecorvable error die audio output problems. Player should abort playback.
};

inline QString playStateToName(PlayState st) {
    switch (st) {
    case Playing:
        return "Playing";
    case Paused:
        return "Paused";
    case Stopped:
        return "Stopped";
    case Buffering:
        return "Buffering";
    case NormalError:
        return "NormalError";
    case FatalError:
        return "FatalError";
    }
}

//Audio channels enum.
enum ChannelPosition
{
    CHAN_NULL         = 0x00,   /*!< No channel */
    CHAN_FRONT_LEFT   = 0x01,   /*!< Front left channel */
    CHAN_FRONT_RIGHT  = 0x02,   /*!< Front right channel */
    CHAN_REAR_LEFT    = 0x04,   /*!< Rear left channel */
    CHAN_REAR_RIGHT   = 0x08,   /*!< Rear right channel */
    CHAN_FRONT_CENTER = 0x10,   /*!< Front center channel */
    CHAN_REAR_CENTER  = 0x20,   /*!< Rear center channel */
    CHAN_SIDE_LEFT    = 0x40,   /*!< Side left channel */
    CHAN_SIDE_RIGHT   = 0x80,   /*!< Side right channel */
    CHAN_LFE          = 0x100,  /*!< Low-frequency effects channel */
};


} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
#endif // PHOENIXBACKEND_GLOBAL_H
