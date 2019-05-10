#ifndef LIB_PHOENIXPLAYER_UTILITY_H
#define LIB_PHOENIXPLAYER_UTILITY_H

#include <QObject>
#include <QTextCodec>
#include "SingletonPointer.h"

class QQmlEngine;
namespace PhoenixPlayer {

class PPUtility : public QObject
{
    Q_OBJECT
    Q_ENUMS(NetworkType)
    Q_PROPERTY(NetworkType networkType READ getNetworkType)
public:
    enum NetworkType {
        TypeUnknown = 0x00,
        TypeMobile,
        TypeEthernet,
        TypeWLAN
    };

private:
    explicit PPUtility(QObject *parent = Q_NULLPTR);
    static PPUtility *createInstance();

public:
    static PPUtility *instance();
    virtual ~PPUtility();
    void setQQmlEngine(QQmlEngine *engine = Q_NULLPTR);

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
#endif // LIB_PHOENIXPLAYER_UTILITY_H
