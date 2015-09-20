#ifndef DECODERHOST_H
#define DECODERHOST_H

#include "PluginHost.h"
#include "Common.h"

namespace PhoenixPlayer {
namespace Decoder {
class DecoderHost : public PluginHost
{
    Q_OBJECT
public:
    explicit DecoderHost(const QString &libraryFile, const QJsonObject &metaData, QObject *parent = 0)
        : PluginHost(libraryFile, metaData, parent) {}
    explicit DecoderHost(const QString &libraryFile, QObject *parent = 0)
        : PluginHost(libraryFile, parent) {}
    virtual ~DecoderHost() {}

    // PluginHost interface
public:
    Common::PluginType type() const {
        return Common::PluginDecoder;
    }
};

} //Decoder
} //PhoenixPlayer
#endif // DECODERHOST_H
