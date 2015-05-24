#ifndef PLUGINHOST_H
#define PLUGINHOST_H

#include <QObject>

#include "Common.h"

class QPluginLoader;
namespace PhoenixPlayer {

class PluginHost : public QObject
{
    Q_OBJECT
public:
    explicit PluginHost(const QString &libraryFile,QObject *parent = 0);
    virtual ~PluginHost();

    Common::PluginType type() const;
    ///
    /// \brief hash 插件hash值，为插件库文件路径的hash值
    /// \return
    ///
    QString hash() const;
    QString name() const;
    QString version() const;
    QString description() const;
    QString libraryFile() const;
    bool isLoaded();
    void unLoad();
    bool isValid();

    QObject *instance();

    bool operator == (const PluginHost &other);
private:
    Common::PluginType mPluginType;
    QString mLibraryFile;
    QString mHash;
    QString mName;
    QString mVersion;
    QString mDescription;
    bool mValid;
    QPluginLoader *mPluginLoader;
    QObject *mPluginObject;

};
} //PhoenixPlayer
#endif // PLUGINHOST_H
