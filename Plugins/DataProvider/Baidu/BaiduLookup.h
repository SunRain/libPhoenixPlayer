#ifndef BAIDULOOKUP_H
#define BAIDULOOKUP_H

#include <QObject>
#include <QByteArray>

#include "libphoenixplayer_global.h"
#include "DataProvider/IMetadataLookup.h"

class QTimer;
class QTextCodec;

namespace QCurl {
    class QCNetworkAccessManager;
    class QCNetworkSyncReply;
    class QCNetworkRequest;
}

namespace PhoenixPlayer{

    class AudioMetaObject;
    namespace DataProvider {

        namespace BaiduLookup {

class LIBPHOENIXPLAYER_EXPORT BaiduLookup : public IMetadataLookup
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.MetadataLookup.BaiduLyricsLookup" FILE "lyricsbaidu.json")
    Q_INTERFACES(PhoenixPlayer::BasePlugin)
public:
    BaiduLookup(QObject *parent = Q_NULLPTR);
    virtual ~BaiduLookup() override;

//     // ILyricsLookup interface
//public:
//    void lookup(const AudioMetaObject &object);
//    bool supportLookup(LookupType type);


    // BasePlugin interface
public:
    PluginProperty property() const Q_DECL_OVERRIDE;

    // IDataProvider interface
public:
    bool support(SupportedTypes type) const Q_DECL_OVERRIDE;

    // IMetadataLookup interface
public:
    QByteArray lookup(const AudioMetaObject &object, SupportedType type, QString *errorMsg) const Q_DECL_OVERRIDE;


private slots:
//    void dlFailed(const QUrl &requestedUrl, const QString &error);
//    void dlSucceed(const QUrl &requestedUrl, const QByteArray &replyData);
    QString formatStr(const QString &in) const;

    void setRequestHeader(QCurl::QCNetworkRequest *req) const;

    QByteArray networkCall(const QCurl::QCNetworkRequest &request, QString *errorMsg) const;



private:
    QTextCodec                      *m_GBKCodec = Q_NULLPTR;
//    BaseNetworkLookup   *m_NDL      = Q_NULLPTR;
    QCurl::QCNetworkAccessManager   *m_network  = Q_NULLPTR;
//    QCurl::QCNetworkSyncReply       *m_reply    = Q_NULLPTR;
    QTimer                          *m_timer    = Q_NULLPTR;
//    bool m_requestAborted = false;

//    bool                m_isLrcidDL = false;

};

} //BaiduLookup
} //DataProvider
} //PhoenixPlayer
#endif // BAIDULOOKUP_H
