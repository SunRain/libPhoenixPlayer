
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QTimer>

#include <QByteArray>

#include "MetadataLookup/BaseNetworkLookup.h"

namespace PhoenixPlayer {
namespace MetadataLookup {

BaseNetworkLookup::BaseNetworkLookup(QObject *parent) : QObject(parent)
{
    mNetwork = new QNetworkAccessManager(this);
    mReply = 0;
    mInterval = 10000;
    mFailEmitted = false;
    mRequestAborted = false;

    mTimer = new QTimer(this);
    mTimer->setSingleShot (true);
    connect (mTimer, &QTimer::timeout, [this] () {
        mRequestAborted = true;
        if (mReply)
            mReply->abort ();
    });
}

BaseNetworkLookup::~BaseNetworkLookup()
{
    qDebug()<<__FUNCTION__;

    if (mReply) {
        mReply->abort ();
        mReply->deleteLater ();
    }

    if (mNetwork)
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

void BaseNetworkLookup::setInterval(int msec)
{
    mInterval = msec;
}

bool BaseNetworkLookup::startLookup(bool watchTimeout)
{
    qDebug()<<__FUNCTION__;
    if (mUrl.isEmpty ())
        return false;

    QUrl url(mUrl);
    if (url.isEmpty ())
        return false;

    if (mReply) {
        mReply->abort ();
        qDebug()<<"====== mReply is running "<<mReply->isRunning ();
        mRequestAborted = true;
    }
    mFailEmitted = false;

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
    if (watchTimeout) {
        mRequestAborted = false;
        mTimer->start (mInterval);
    }

    if (mReply) {
        connect (mReply, &QNetworkReply::finished,
                 this, &BaseNetworkLookup::readReplyData);
    }
    return true;
}

void BaseNetworkLookup::readReplyData()
{
    mTimer->stop ();
    QUrl url = mReply->request ().url ();
    if (mRequestAborted) {
        mReply->deleteLater ();
        mReply = 0;
        if (!mFailEmitted) {
            mFailEmitted = true;
            emit failed (url, QString("Aborted du to time out"));
        }
        return;
    }
    QNetworkReply::NetworkError error = mReply->error ();
    if (error != QNetworkReply::NetworkError::NoError) {
        QString errorStr = mReply->errorString ();
        mReply->deleteLater ();
        mReply = 0;
        if (!mFailEmitted) {
            mFailEmitted = true;
            emit failed (url, errorStr);
        }
        return;
    }
    qDebug()<<"===  BaseNetworkLookup  succeed";

    QByteArray qba = mReply->readAll ();
    mReply->deleteLater ();
    mReply = 0;
    emit succeed (url, qba);
}
} //Lyrics
} //PhoenixPlayer
