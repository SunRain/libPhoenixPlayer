#ifndef OUTPUTHOST_H
#define OUTPUTHOST_H

#include "PluginHost.h"
#include "PPCommon.h"

namespace PhoenixPlayer {
namespace OutPut {
class OutPutHost : public PluginHost
{
    Q_OBJECT
public:
    explicit OutPutHost(const QString &libraryFile, const QJsonObject &metaData, QObject *parent = 0)
        : PluginHost(libraryFile, metaData, parent) {}
    explicit OutPutHost(const QString &libraryFile, QObject *parent = 0)
        : PluginHost(libraryFile, parent) {}
    virtual ~OutPutHost() {}

    // PluginHost interface
public:
    PPCommon::PluginType type() const {
        return PPCommon::PluginOutPut;
    }
};
} //OutPut
} //PhoenixPlayer
#endif // OUTPUTHOST_H
