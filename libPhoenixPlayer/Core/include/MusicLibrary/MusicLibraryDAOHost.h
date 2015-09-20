#ifndef MUSICLIBRARYDAOHOST_H
#define MUSICLIBRARYDAOHOST_H

#include "PluginHost.h"
#include "Common.h"

namespace PhoenixPlayer {
namespace MusicLibrary {
class MusicLibraryDAOHost : public PluginHost
{
    Q_OBJECT
public:
    explicit MusicLibraryDAOHost(const QString &libraryFile, const QJsonObject &metaData, QObject *parent = 0)
        : PluginHost(libraryFile, metaData, parent) {}
    explicit MusicLibraryDAOHost(const QString &libraryFile, QObject *parent = 0)
        : PluginHost(libraryFile, parent) {}
    virtual ~MusicLibraryDAOHost() {}

    // PluginHost interface
public:
    Common::PluginType type() const {
        return Common::PluginMusicLibraryDAO;
    }
};
} //MusicLibrary
} //PhoenixPlayer
#endif // MUSICLIBRARYDAOHOST_H
