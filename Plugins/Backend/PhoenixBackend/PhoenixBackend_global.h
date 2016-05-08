#ifndef PHOENIXBACKEND_GLOBAL_H
#define PHOENIXBACKEND_GLOBAL_H

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

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
#endif // PHOENIXBACKEND_GLOBAL_H
