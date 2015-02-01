
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#include <QByteArray>

#include "BaseNetworkLookup.h"

namespace PhoenixPlayer {
namespace Lyrics {

BaseNetworkLookup::BaseNetworkLookup(QObject *parent) : QObject(parent)
{
    mNetwork = new QNetworkAccessManager(this);
}

BaseNetworkLookup::~BaseNetworkLookup()
{
    if (mNetwork != nullptr)
        mNetwork->deleteLater ();
}

void BaseNetworkLookup::setUrl(const QString &url)
{
    mUrl = url;
}

void BaseNetworkLookup::setRequestType(BaseNetworkLookup::RequestType type)
{
    mRequestType = type;
}

bool BaseNetworkLookup::startLookup()
{
    if (mUrl.isEmpty ())
        return false;

    QUrl url(mUrl);

    qDebug()<<"************ url "<<url<<" ******************";

    if (url.isEmpty ())
        return false;

    QNetworkReply *reply = nullptr;

    switch (mRequestType) {
    case RequestType::RequestGet: {
        qDebug()<<"Get "<< url.toString ();
        QNetworkRequest request(url);
        reply = mNetwork->get (request);
        break;
    }
    case RequestType::RequestPut: {
        QByteArray qba(url.query (QUrl::FullyEncoded).toLocal8Bit ());
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          "application/x-www-form-urlencoded");
        request.setHeader(QNetworkRequest::ContentLengthHeader,
                          QByteArray::number(qba.length()));
        qDebug("post data [%s] to [%s]",
               qba.constData(),
               url.toString().toLocal8Bit().constData());
        reply = mNetwork->post (request, qba);
        break;
    }
    default:
        break;
    }

    if (reply) {
        //请求成功
        connect (reply,
                 &QNetworkReply::finished,
                 [this, reply]() {
            qDebug()<<"===  BaseNetworkLookup  finished";
            QNetworkReply::NetworkError error = reply->error ();
            if (error != QNetworkReply::NetworkError::NoError)
                return;

            qDebug()<<"===  BaseNetworkLookup  succeed";

            QByteArray qba = reply->readAll ();
            emit succeed (reply->request ().url (), qba);
            reply->deleteLater ();

        });

        //请求失败
        connect (reply,
                 //解决信号和方法函数重载问题
                 static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
                 [this, reply](QNetworkReply::NetworkError error) {
            Q_UNUSED(error)

             qDebug()<<"===  BaseNetworkLookup  error "<<reply->errorString ();

            emit failed (reply->request ().url (), reply->errorString ());
             reply->deleteLater ();

        });
        return true;
    }
    return false;
}

} //Lyrics
} //PhoenixPlayer
