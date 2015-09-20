#ifndef PLUGINHOST_H
#define PLUGINHOST_H

#include <QObject>
#include <QDebug>
#include <QPluginLoader>

#include "Common.h"

namespace PhoenixPlayer {

class PluginHost : public QObject
{
    Q_OBJECT
public:
    explicit PluginHost(const QString &libraryFile,QObject *parent = 0);
    ///
    /// \brief PluginHost
    /// \param libraryFile 插件库文件路径
    /// \param metaData 插件JSON格式的配置文件
    /// \param parent
    ///
    explicit PluginHost(const QString &libraryFile, const QJsonObject &metaData, QObject *parent = 0);

    virtual ~PluginHost();

    virtual Common::PluginType type() const = 0;
//    ///
//    /// \brief hash 插件hash值，为插件库文件路径的hash值
//    /// \return
//    ///
//    QString hash() const;
    QString name() const;
    QString version() const;
    QString description() const;
    QString libraryFile() const;
    QString configFile() const;
    bool isLoaded();
    bool unLoad();
    void forceUnload();
    bool isValid();

    template <class T>
    T *instance() {
        qDebug()<<Q_FUNC_INFO;
        if (!isLoaded ()) {
            if (!m_pluginLoader)
                m_pluginLoader = new QPluginLoader(m_libraryFile, this);
            m_pluginObject = m_pluginLoader->instance ();
        } else {
            if (!m_pluginObject) {
                if (!unLoad ())
                    forceUnload ();
                if (!m_pluginLoader)
                    m_pluginLoader = new QPluginLoader(m_libraryFile, this);
                m_pluginObject = m_pluginLoader->instance ();
            }
        }
        qDebug()<<Q_FUNC_INFO<<" here mPluginObject is nullptr "<<(m_pluginObject == 0);
        if (m_pluginObject)
            return qobject_cast<T *>(m_pluginObject);
        return nullptr;
    }

//    bool operator == (const PluginHost &other);
protected:
    inline QJsonObject metaObject ();

private:
//    Common::PluginType m_pluginType;
    QString m_libraryFile;
//    QString m_hash;
    QString m_name;
    QString m_version;
    QString m_description;
    QString m_configFile;
    bool m_valid;
    QPluginLoader *m_pluginLoader;
    QObject *m_pluginObject;
    QJsonObject m_metaObject;

};
} //PhoenixPlayer
#endif // PLUGINHOST_H
