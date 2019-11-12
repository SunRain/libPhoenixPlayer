//#ifndef PLUGINHOST_H
//#define PLUGINHOST_H

//#include <QObject>
//#include <QDebug>
//#include <QPluginLoader>

//#include "PPCommon.h"

//namespace PhoenixPlayer {

//class PluginHost : public QObject
//{
//    Q_OBJECT
//public:
//    explicit PluginHost(const QString &libraryFile,QObject *parent = 0);
//    ///
//    /// \brief PluginHost
//    /// \param libraryFile 插件库文件路径
//    /// \param metaData 插件JSON格式的配置文件
//    /// \param parent
//    ///
//    explicit PluginHost(const QString &libraryFile, const QJsonObject &metaData, QObject *parent = 0);

//    virtual ~PluginHost();

//    virtual PPCommon::PluginType type() const = 0;

//    QString name() const;
//    QString version() const;
//    QString description() const;
//    QString libraryFile() const;
//    QString configFile() const;

//    inline bool isLoaded() {
//        if (!m_pluginLoader)
//            return false;
//        return m_pluginLoader->isLoaded ();
//    }

//    inline bool unLoad() {
//        qDebug()<<Q_FUNC_INFO;
//        if (!m_pluginLoader)
//            m_pluginLoader = new QPluginLoader(m_libraryFile, this);
//        if (m_pluginLoader->isLoaded ()) {
//            if (!m_pluginLoader->unload ()) {
//                qDebug()<<Q_FUNC_INFO<<" fail to unload plugin due to "<<m_pluginLoader->errorString ();
//                return false;
//            }
//        }
//        if (m_pluginLoader)
//            m_pluginLoader->deleteLater ();
//        m_pluginLoader = nullptr;
//        return true;
//    }

//    inline void forceUnload() {
//        qDebug()<<Q_FUNC_INFO;
//        if (!m_pluginLoader)
//            m_pluginLoader = new QPluginLoader(m_libraryFile, this);
//        if (m_pluginLoader->isLoaded ()) {
//            if (!m_pluginLoader->unload ())
//                qDebug()<<Q_FUNC_INFO<<" fail to unload plugin due to "<<m_pluginLoader->errorString ();
//        }
//        if (m_pluginObject)
//            m_pluginObject->deleteLater ();
//        m_pluginObject = nullptr;
//        if (m_pluginLoader)
//            m_pluginLoader->deleteLater ();
//        m_pluginLoader = nullptr;
//    }

//    inline bool isValid() {
//        return m_valid;
//    }

//    template <class T>
//    T *instance() {
//        qDebug()<<Q_FUNC_INFO;
//        if (!isLoaded ()) {
//            if (!m_pluginLoader)
//                m_pluginLoader = new QPluginLoader(m_libraryFile, this);
//            m_pluginObject = m_pluginLoader->instance ();
//        } else {
//            if (!m_pluginObject) {
//                if (!unLoad ())
//                    forceUnload ();
//                if (!m_pluginLoader)
//                    m_pluginLoader = new QPluginLoader(m_libraryFile, this);
//                m_pluginObject = m_pluginLoader->instance ();
//            }
//        }
//        qDebug()<<Q_FUNC_INFO<<" here mPluginObject is nullptr "<<(m_pluginObject == 0);
//        if (m_pluginObject)
//            return qobject_cast<T *>(m_pluginObject);
//        return nullptr;
//    }
//protected:
//    inline QJsonObject metaObject ();

//private:
////    Common::PluginType m_pluginType;
//    QString m_libraryFile;
////    QString m_hash;
//    QString m_name;
//    QString m_version;
//    QString m_description;
//    QString m_configFile;
//    bool m_valid;
//    QPluginLoader *m_pluginLoader;
//    QObject *m_pluginObject;
//    QJsonObject m_metaObject;

//};
//} //PhoenixPlayer
//#endif // PLUGINHOST_H
