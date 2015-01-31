#ifndef BASENETWORKLOOKUP_H
#define BASENETWORKLOOKUP_H

#include <QObject>

class QNetworkAccessManager;
class QByteArray;

namespace PhoenixPlayer {
namespace Lyrics {

class BaseNetworkLookup : public QObject
{
    Q_OBJECT
public:
    enum RequestType {
        RequestPut = 0x0,
        RequestGet
    };

    explicit BaseNetworkLookup(QObject *parent = 0);
    ~BaseNetworkLookup();

//    virtual QString getUrl() = 0;
//    virtual RequestType getRequestType() = 0;
    void setUrl(const QString &url);
    void setRequestType(RequestType type);
    bool startLookup();
signals:
    void succeed(const QUrl &requestedUrl, const QByteArray &replyData);
    void failed(const QUrl &requestedUrl, const QString &error);
public slots:

private:
    QNetworkAccessManager *mNetwork;
    QString mUrl;
    RequestType mRequestType;
};
} //Lyrics
} //PhoenixPlayer

#endif // BASENETWORKLOOKUP_H
