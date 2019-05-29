#ifndef MUSICTAGPARSERHOST_H
#define MUSICTAGPARSERHOST_H

#include "PluginHost.h"
#include "PPCommon.h"

namespace PhoenixPlayer {
namespace MusicLibrary {
class MusicTagParserHost : public PluginHost
{
    Q_OBJECT
public:
    explicit MusicTagParserHost(const QString &libraryFile, const QJsonObject &metaData, QObject *parent = 0)
        : PluginHost(libraryFile, metaData, parent) {}
    explicit MusicTagParserHost(const QString &libraryFile, QObject *parent = 0)
        : PluginHost(libraryFile, parent) {}
    virtual ~MusicTagParserHost() {}

    // PluginHost interface
public:
    PPCommon::PluginType type() const {
        return PPCommon::PluginMusicTagParser;
    }
};
} //MusicLibrary
} //PhoenixPlayer
#endif // MUSICTAGPARSERHOST_H
