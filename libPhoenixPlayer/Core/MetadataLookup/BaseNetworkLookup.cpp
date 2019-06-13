#include "MetadataLookup/BaseNetworkLookup.h"

#include <QUrl>
#include <QDebug>
#include <QTimer>
#include <QByteArray>

#include "QCNetworkAccessManager.h"
#include "QCNetworkSyncReply.h"

namespace PhoenixPlayer {
namespace MetadataLookup {

using namespace QCurl;

BaseNetworkLookup::BaseNetworkLookup(QObject *parent) : QObject(parent)
{
    m_network = new QCNetworkAccessManager(this);
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
        m_requestAborted = true;
    }
    m_failEmitted = false;
    if (!cookieFile().isEmpty()) {
        m_network->setCookieFilePath(cookieFile());
    }
    QByteArray replyData;
    QCNetworkRequest request(url);
    request.setRawHeader("User-Agent",
                         "Mozilla/5.0 (Windows;U;Windows NT 5.1;zh-CN;rv:1.9.2.9) Gecko/20100101 Firefox/43.0");
    request.setRawHeader("Accept",
                         "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");

    if (m_requestType == RequestType::RequestGet) {
        qDebug()<<"Get "<< url.toString ();
        m_reply = m_network->create(request);
    } else {
        QByteArray qba(url.query(QUrl::FullyEncoded).toUtf8());
        request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
        request.setRawHeader("Content-Length", QByteArray::number(qba.length()));
        qDebug("post data [%s] to [%s]",
               qba.constData(),
               url.toString().toUtf8().constData());
        m_reply = m_network->create(request);
        m_reply->setPostData(qba);
    }

    m_reply->setWriteFunction([&](char *buffer, size_t size) ->size_t {
        replyData.append(buffer, static_cast<int>(size));
        return size;
    });
    m_reply->setCustomHeaderFunction([&](char *buffer, size_t size)->size_t {
        if (!replyData.isEmpty()) {
            const QByteArray header(buffer, static_cast<int>(size));
            const int pos = replyData.indexOf(header);
            if (pos >= 0) {
                replyData.remove(pos, static_cast<int>(size));
            }
        }
        return size;
    });

    if (watchTimeout) {
        m_requestAborted = false;
        m_timer->start(m_interval);
    }

    m_reply->perform();

    if (m_timer->isActive()) {
        m_timer->stop();
    }

    url = m_reply->request().url();
    const NetworkError error = m_reply->error();
    const QString errorStr = m_reply->errorString();
    m_reply->deleteLater();
    m_reply = Q_NULLPTR;
    if (error != NetworkNoError) {
        if (!m_failEmitted) {
            m_failEmitted = true;
            emit failed(url, errorStr);
        }
        return false;
    }
    qDebug()<<Q_FUNC_INFO<<"=== succeed with result "<<replyData;

    emit succeed(url, replyData);
    return true;
}

void BaseNetworkLookup::doTimeout()
{
    m_requestAborted = true;
    if (m_reply)
        m_reply->abort ();
}
} //Lyrics
} //PhoenixPlayer
