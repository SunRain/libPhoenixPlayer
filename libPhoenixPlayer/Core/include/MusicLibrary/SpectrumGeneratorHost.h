#ifndef SPECTRUMGENERATORHOST_H
#define SPECTRUMGENERATORHOST_H

#include "PluginHost.h"
#include "PPCommon.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

class SpectrumGeneratorHost : public PluginHost
{
    Q_OBJECT
public:
    explicit SpectrumGeneratorHost(const QString &libraryFile, const QJsonObject &metaData, QObject *parent = Q_NULLPTR)
        : PluginHost(libraryFile, metaData, parent) {}
    explicit SpectrumGeneratorHost(const QString &libraryFile, QObject *parent = Q_NULLPTR)
        : PluginHost(libraryFile, parent) {}
    virtual ~SpectrumGeneratorHost() {}

    // PluginHost interface
public:
    PPCommon::PluginType type() const {
        return PPCommon::PluginSpectrumGenerator;
    }
};




} //MusicLibrary
} //PhoenixPlayer
#endif // SPECTRUMGENERATORHOST_H
