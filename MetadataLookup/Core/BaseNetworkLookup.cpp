
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>

#include <QByteArray>

#include "BaseNetworkLookup.h"

namespace PhoenixPlayer {
namespace MetadataLookup {

BaseNetworkLookup::BaseNetworkLookup(QObject *parent) : QObject(parent)
{
    mNetwork = new QNetworkAccessManager(this);
}

BaseNetworkLookup::~BaseNetworkLookup()
{
    qDebug()<<__FUNCTION__;

    if (mNetwork != 0)
        mNetwork->deleteLater ();

     qDebug()<<"after"<<__FUNCTION__;
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
    qDebug()<<__FUNCTION__;
    if (mUrl.isEmpty ())
        return false;

    QUrl url(mUrl);
    if (url.isEmpty ())
        return false;

    QNetworkReply *reply = 0;

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
                 [=]() {
            qDebug()<<"===  BaseNetworkLookup  finished";
            QNetworkReply::NetworkError error = reply->error ();
            if (error != QNetworkReply::NetworkError::NoError) {
                emit failed (QUrl(), reply->errorString ());
                reply->deleteLater ();
                return;
            }
            qDebug()<<"===  BaseNetworkLookup  succeed";

            QByteArray qba = reply->readAll ();
            QUrl url(reply->request ().url ());
            reply->deleteLater ();
            emit succeed (url, qba);
        });

        //请求失败
        connect (reply,
                 //解决信号和方法函数重载问题
                 static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
                 [=](QNetworkReply::NetworkError error) {
            Q_UNUSED(error)

//            if (!reply) {
//                emit failed (QUrl(), QString::number ((int)error));
//                return;
//            }

            QUrl url = reply->request ().url ();
            QString errorStr = reply->errorString ();

            qDebug()<<"===  BaseNetworkLookup  error "<<errorStr;

            reply->deleteLater ();
            emit failed (url, errorStr);
        });
        return true;
    }
    return false;
}

} //Lyrics
} //PhoenixPlayer
