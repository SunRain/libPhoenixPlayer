#ifndef BASENETWORKLOOKUP_H
#define BASENETWORKLOOKUP_H

#include <QObject>

namespace QCurl {
    class QCNetworkAccessManager;
    class QCNetworkSyncReply;
}

class QByteArray;
class QTimer;

namespace PhoenixPlayer {
namespace MetadataLookup {

///
/// \brief The BaseNetworkLookup class
/// Sync network lookup
///
class BaseNetworkLookup : public QObject
{
    Q_OBJECT
public:
    enum RequestType {
        RequestPut = 0x0,
        RequestGet
    };

    explicit BaseNetworkLookup(QObject *parent = Q_NULLPTR);
    virtual ~BaseNetworkLookup();

    void setUrl(const QString &url);
    void setRequestType(RequestType type);

    ///
    /// \brief setInterval msec to interrupt the request
    /// \param msec  default 10000
    ///
    void setInterval(int msec);

    bool startLookup(bool watchTimeout = true);

protected:
    ///
    /// \brief cookieFile
    /// \return valid file path to save cookie data. Empty string will not use cookie
    ///
    virtual QString cookieFile() const
    {
        return QString();
    }

signals:
    void succeed(const QUrl &requestedUrl, const QByteArray &replyData);
    void failed(const QUrl &requestedUrl, const QString &error);

private:
    void doTimeout();

private:
    QCurl::QCNetworkAccessManager           *m_network;
    QCurl::QCNetworkSyncReply               *m_reply;
    QTimer                                  *m_timer;
    QString                                 m_url;
    RequestType                             m_requestType;
    int                                     m_interval;
    bool                                    m_failEmitted;
    bool                                    m_requestAborted;
};
} //Lyrics
} //PhoenixPlayer

#endif // BASENETWORKLOOKUP_H
