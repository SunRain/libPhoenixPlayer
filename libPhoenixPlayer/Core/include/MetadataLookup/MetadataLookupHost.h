#ifndef METADATALOOKUPHOST_H
#define METADATALOOKUPHOST_H

#include "PluginHost.h"
#include "PPCommon.h"

namespace PhoenixPlayer{
namespace MetadataLookup {
class MetadataLookupHost : public PluginHost
{
    Q_OBJECT
public:
    explicit MetadataLookupHost(const QString &libraryFile, const QJsonObject &metaData, QObject *parent = 0)
        : PluginHost(libraryFile, metaData, parent) {}
    explicit MetadataLookupHost(const QString &libraryFile, QObject *parent = 0)
        : PluginHost(libraryFile, parent) {}
    virtual ~MetadataLookupHost() {}

    // PluginHost interface
public:
    PPCommon::PluginType type() const {
        return PPCommon::PluginMetadataLookup;
    }
};
} //MetadataLookup
} //PhoenixPlayer
#endif // METADATALOOKUPHOST_H
