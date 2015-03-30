
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
    qDebug()<<__FUNCTION__;
    if (mUrl.isEmpty ())
        return false;

    QUrl url(mUrl);
    if (url.isEmpty ())
        return false;

    if (mReply) {
        mReply->abort ();
        delete mReply;
        mReply = nullptr;
    }

    switch (mRequestType) {
    case RequestType::RequestGet: {
        qDebug()<<"Get "<< url.toString ();
        QNetworkRequest request(url);
        qDebug()<<"..............................................";
        mReply = mNetwork->get (request);

        qDebug()<<"########### "<<(mReply == 0);
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
            QUrl url(mReply->request ().url ());
//            mReply->deleteLater ();
            emit succeed (url, qba);
        });

        //请求失败
        connect (mReply,
                 //解决信号和方法函数重载问题
                 static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
                 [this](QNetworkReply::NetworkError error) {
            Q_UNUSED(error)

            qDebug()<<"===  xxxxxxxxxxxxxxxxxxxxxxxxxxx ";

            QUrl url(mReply->request ().url ());
            QString errorStr(mReply->errorString ());

            qDebug()<<"===  BaseNetworkLookup  error "<<errorStr;

//            mReply->deleteLater ();
            emit failed (url, errorStr);
        });
        return true;
    }
    return false;
}

} //Lyrics
} //PhoenixPlayer
