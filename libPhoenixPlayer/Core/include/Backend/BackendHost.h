#ifndef BACKENDHOST_H
#define BACKENDHOST_H

#include "PluginHost.h"
#include "Common.h"

namespace PhoenixPlayer {
namespace PlayBackend {

class BackendHost : public PluginHost
{
    Q_OBJECT
public:
    explicit BackendHost(const QString &libraryFile, const QJsonObject &metaData, QObject *parent = 0)
        : PluginHost(libraryFile, metaData, parent) {}
    explicit BackendHost(const QString &libraryFile,QObject *parent = 0)
        : PluginHost(libraryFile, parent) {}
    virtual ~BackendHost() {}

    // PluginHost interface
public:
    Common::PluginType type() const {
        return Common::PluginPlayBackend;
    }
};


} //PlayBackend
} //PhoenixPlayer
#endif // BACKENDHOST_H
