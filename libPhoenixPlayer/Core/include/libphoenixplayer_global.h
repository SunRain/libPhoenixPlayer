#ifndef LIBPHOENIXPLAYER_GLOBAL_H
#define LIBPHOENIXPLAYER_GLOBAL_H

#include <qglobal.h>

#if defined(LIBPHOENIXPLAYER_LIBRARY)
#  undef LIBPHOENIXPLAYER_EXPORT
#  define LIBPHOENIXPLAYER_EXPORT Q_DECL_EXPORT
#else
#  undef LIBPHOENIXPLAYER_EXPORT
#  define LIBPHOENIXPLAYER_EXPORT Q_DECL_IMPORT
#endif

#define libPhoenixPlayer PhoenixPlayer::LibPhoenixPlayer::instance();

namespace PhoenixPlayer {
class AudioMetaObject;

void LibPhoenixPlayer_Init();

}
typedef QList<PhoenixPlayer::AudioMetaObject> AudioMetaList;

#endif // LIBPHOENIXPLAYER_GLOBAL_H
