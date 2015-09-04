
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
    m_network = new QNetworkAccessManager(this);
    m_reply = nullptr;
    m_interval = 10000;
    m_failEmitted = false;
    m_requestAborted = false;

    m_timer = new QTimer(this);
    m_timer->setSingleShot (true);
    connect (m_timer, &QTimer::timeout,
             this, &BaseNetworkLookup::doTimeout);
}

BaseNetworkLookup::~BaseNetworkLookup()
{
    qDebug()<<Q_FUNC_INFO;

    if (m_timer->isActive ())
        m_timer->stop ();
    m_timer->deleteLater ();

    if (m_reply) {
        m_reply->abort ();
        m_reply->deleteLater ();
    }

    if (m_network)
        m_network->deleteLater ();

     qDebug()<<"after"<<Q_FUNC_INFO;
}

void BaseNetworkLookup::setUrl(const QString &url)
{
    m_url = url;
}

void BaseNetworkLookup::setRequestType(BaseNetworkLookup::RequestType type)
{
    m_requestType = type;
}

void BaseNetworkLookup::setInterval(int msec)
{
    m_interval = msec;
}

bool BaseNetworkLookup::startLookup(bool watchTimeout)
{
    qDebug()<<Q_FUNC_INFO;
    if (m_url.isEmpty ())
        return false;

    QUrl url(m_url);
    if (url.isEmpty ())
        return false;

    if (m_reply) {
        m_reply->abort ();
        qDebug()<<"====== mReply is running "<<m_reply->isRunning ();
        m_requestAborted = true;
    }
    m_failEmitted = false;

    switch (m_requestType) {
    case RequestType::RequestGet: {
        qDebug()<<"Get "<< url.toString ();
        QNetworkRequest request(url);
        m_reply = m_network->get (request);
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
        m_reply = m_network->post (request, qba);
        break;
    }
    default:
        break;
    }
    if (watchTimeout) {
        m_requestAborted = false;
        m_timer->start (m_interval);
    }

    if (m_reply) {
        connect (m_reply, &QNetworkReply::finished,
                 this, &BaseNetworkLookup::readReplyData);
    }
    return true;
}

void BaseNetworkLookup::doTimeout()
{
    m_requestAborted = true;
    if (m_reply)
        m_reply->abort ();
}

void BaseNetworkLookup::readReplyData()
{
    if (m_timer->isActive ())
        m_timer->stop ();

    QUrl url = m_reply->request ().url ();
    if (m_requestAborted) {
        m_reply->deleteLater ();
        m_reply = nullptr;
        if (!m_failEmitted) {
            m_failEmitted = true;
            emit failed (url, QString("Aborted du to time out"));
        }
        return;
    }
    QNetworkReply::NetworkError error = m_reply->error ();
    if (error != QNetworkReply::NetworkError::NoError) {
        QString errorStr = m_reply->errorString ();
        m_reply->deleteLater ();
        m_reply = nullptr;
        if (!m_failEmitted) {
            m_failEmitted = true;
            emit failed (url, errorStr);
        }
        return;
    }

    QByteArray qba = m_reply->readAll ();
    m_reply->deleteLater ();
    m_reply = nullptr;

    qDebug()<<Q_FUNC_INFO<<"===  BaseNetworkLookup  succeed with result "<<qba;

    emit succeed (url, qba);
}
} //Lyrics
} //PhoenixPlayer
