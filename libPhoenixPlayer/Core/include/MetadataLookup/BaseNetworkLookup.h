#ifndef BASENETWORKLOOKUP_H
#define BASENETWORKLOOKUP_H

#include <QObject>

class QNetworkAccessManager;
class QByteArray;
class QNetworkReply;
class QTimer;

namespace PhoenixPlayer {
namespace MetadataLookup {

class BaseNetworkLookup : public QObject
{
    Q_OBJECT
public:
    enum RequestType {
        RequestPut = 0x0,
        RequestGet
    };

    explicit BaseNetworkLookup(QObject *parent = 0);
    virtual ~BaseNetworkLookup();

    void setUrl(const QString &url);
    void setRequestType(RequestType type);
    void setInterval(int msec);
    bool startLookup(bool watchTimeout = true);
signals:
    void succeed(const QUrl &requestedUrl, const QByteArray &replyData);
    void failed(const QUrl &requestedUrl, const QString &error);
public slots:

private slots:
    void doTimeout();
private:
    void readReplyData();
private:
    QNetworkAccessManager *m_network;
    QNetworkReply *m_reply;
    QString m_url;
    RequestType m_requestType;
    QTimer *m_timer;
    int m_interval;
    bool m_failEmitted;
    bool m_requestAborted;
};
} //Lyrics
} //PhoenixPlayer

#endif // BASENETWORKLOOKUP_H
