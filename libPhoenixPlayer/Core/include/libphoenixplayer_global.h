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

namespace PhoenixPlayer {
class AudioMetaObject;
}
typedef QList<PhoenixPlayer::AudioMetaObject> AudioMetaList;

#endif // LIBPHOENIXPLAYER_GLOBAL_H
