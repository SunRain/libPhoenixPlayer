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
    QString configFile() const;
    bool isLoaded();
    void unLoad();
    bool isValid();

    QObject *instance();

    bool operator == (const PluginHost &other);
private:
    Common::PluginType m_pluginType;
    QString m_libraryFile;
    QString m_hash;
    QString m_name;
    QString m_version;
    QString m_description;
    QString m_configFile;
    bool m_valid;
    QPluginLoader *m_pluginLoader;
    QObject *m_pluginObject;

};
} //PhoenixPlayer
#endif // PLUGINHOST_H
