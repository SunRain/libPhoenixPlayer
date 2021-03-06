#ifndef UTIL_H
#define UTIL_H

#include <QObject>
#include <QTextCodec>
#include "SingletonPointer.h"

class QQmlEngine;
namespace PhoenixPlayer {

class Util : public QObject
{
    Q_OBJECT
    Q_ENUMS(NetworkType)
    Q_PROPERTY(NetworkType networkType READ getNetworkType)

    DECLARE_SINGLETON_POINTER(Util)
public:
    enum NetworkType {
        TypeUnknown = 0x00,
        TypeMobile,
        TypeEthernet,
        TypeWLAN
    };

public:
//#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
//    static Util *instance();
//#endif
//    explicit Util(QObject *parent = 0);
    virtual ~Util();
    void setQQmlEngine(QQmlEngine *engine = 0);

    Q_INVOKABLE static QString calculateHash(const QString &str);

    static QTextCodec *localeDefaultCodec();

    Q_INVOKABLE static QString formateSongDuration(int time);
    Q_INVOKABLE static QString formateFileSize(int size);

    static QStringList getAddonDirList();

    NetworkType getNetworkType();

    ///
    /// \brief supportMDLookupTypeJson
    /// 返回支持的metadata lookup类型，仅仅适用于metadata lookup插件
    /// \param pluginHash
    ///
//    Q_INVOKABLE QString supportMDLookupTypeJsonStr(const QString &pluginHash);

signals:

public slots:

private:
    QQmlEngine *m_QQmlEngine;
};
} //PhoenixPlayer
#endif // UTIL_H
