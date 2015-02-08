
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#include <QByteArray>

#include "BaseNetworkLookup.h"

namespace PhoenixPlayer {
namespace MetadataLookup {

BaseNetworkLookup::BaseNetworkLookup(QObject *parent) : QObject(parent)
{
    mNetwork = new QNetworkAccessManager(this);
    mReply = nullptr;
}

BaseNetworkLookup::~BaseNetworkLookup()
{
    qDebug()<<__FUNCTION__;

    if (mReply) {
        mReply->abort ();
        mReply->deleteLater ();
    }

    if (mNetwork != nullptr)
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
    if (mUrl.isEmpty ())
        return false;

    QUrl url(mUrl);

    qDebug()<<"************ url "<<url<<" ******************";

    if (url.isEmpty ())
        return false;

    switch (mRequestType) {
    case RequestType::RequestGet: {
        qDebug()<<"Get "<< url.toString ();
        QNetworkRequest request(url);
        mReply = mNetwork->get (request);
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
        mReply = mNetwork->post (request, qba);
        break;
    }
    default:
        break;
    }

    if (mReply) {
        //请求成功
        connect (mReply,
                 &QNetworkReply::finished,
                 [this]() {
            qDebug()<<"===  BaseNetworkLookup  finished";
            QNetworkReply::NetworkError error = mReply->error ();
            if (error != QNetworkReply::NetworkError::NoError)
                return;

            qDebug()<<"===  BaseNetworkLookup  succeed";

            QByteArray qba = mReply->readAll ();
            emit succeed (mReply->request ().url (), qba);
            mReply->deleteLater ();

        });

        //请求失败
        connect (mReply,
                 //解决信号和方法函数重载问题
                 static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
                 [this](QNetworkReply::NetworkError error) {
            Q_UNUSED(error)

             qDebug()<<"===  BaseNetworkLookup  error "<<mReply->errorString ();

            emit failed (mReply->request ().url (), mReply->errorString ());
             mReply->deleteLater ();

        });
        return true;
    }
    return false;
}

} //Lyrics
} //PhoenixPlayer
