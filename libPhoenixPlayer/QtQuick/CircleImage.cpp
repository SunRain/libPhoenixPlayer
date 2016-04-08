#include "CircleImage.h"

#include <QUrl>
#include <QScopedPointer>
#include <QPixmap>
#include <QImage>
#include <QBrush>
#include <QPainter>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

#include "SingletonPointer.h"
#include "Settings.h"
#include "Utility.h"
#include "LibPhoenixPlayerMain.h"


namespace PhoenixPlayer {
namespace QmlPlugin {

CircleImage::CircleImage(QQuickPaintedItem *parent)
    : QQuickPaintedItem(parent)
{
    m_source = QUrl();
    m_status = Status::Null;
    m_networkAccessManager = 0;
    m_reply = 0;
    m_image = 0;
    m_file = 0;
    m_requestAborted = false;
    m_cache = true;
    m_byteArray = QByteArray();
    m_settings = phoenixPlayerLib->settings ();//Settings::instance ();
}

CircleImage::~CircleImage()
{
}

void CircleImage::paint(QPainter *painter)
{
//    if (mSource.isEmpty() || !mSource.isValid()) {
//        this->setStatus (Status::Error);
//        return;
//    }
//    QString str = mSource.toLocalFile();
//    if (str.isEmpty())
//        str = mSource.toString();
//    if (str.isEmpty()) {
//        this->setStatus (Status::Error);
//        return;
//    }
//    //remove qrc: from file uri to fix image load
//    if (str.startsWith("qrc:"))
//        str = str.mid(3, str.length() - 3);

//    QImage image(str);
    if (m_image == 0 || m_image->isNull ()) {
        qDebug()<<__FUNCTION__<<" Image is null";
        this->setStatus (Status::Error);
        return;
    }

//    this->setStatus (Status::Loading);

    //取得一个圆的最短长度
    int width = m_image->width();
    int targetWidth = this->width();
    int height = m_image->height();
    int targetHeight = this->height();
    QImage img = m_image->scaled(targetWidth, targetHeight, Qt::KeepAspectRatioByExpanding);

    width = img.width();
    height = img.height();

    int radius = qMin(width, height);

    //make a transparent image
    QImage out(radius, radius, QImage::Format_ARGB32_Premultiplied);
    out.fill(Qt::transparent);

    QBrush b(img);
    QPainter p(&out);
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(b);
    p.drawEllipse((targetWidth - radius)/2, (targetHeight - radius)/2,
                  radius, radius);

    this->setImplicitWidth(out.width());
    this->setImplicitHeight(out.height());

    QPixmap pixmap  = QPixmap::fromImage(out);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->drawPixmap(boundingRect().toRect(), pixmap);

    this->setStatus (Status::Ready);
}

QUrl CircleImage::source() const
{
    return m_source;
}

void CircleImage::setSource(const QUrl &source)
{
//    qDebug()<<__FUNCTION__<<" source set to "<<source
//              <<" msource is "<<mSource;

    if (m_source == source) {
        return;
    }
    m_source = source;
    if (m_source.isEmpty() || !m_source.isValid()) {
        this->setStatus (Status::Error);
        return;
    }
    QString str = m_source.toLocalFile();
    if (str.isEmpty())
        str = m_source.toString();
    if (str.isEmpty()) {
        this->setStatus (Status::Error);
        return;
    }

    this->setStatus (Status::Loading);

    if (str.toLower ().startsWith (("http"))) { //url
        downloadFile (m_source);
        return;
    }

    //remove qrc: from file uri to fix image load
    if (str.startsWith("qrc:")) {
        str = str.mid(3, str.length() - 3);
    }
    if (fillImage (str)) {
        this->update ();
    }
}

CircleImage::Status CircleImage::status() const
{
    return m_status;
}

void CircleImage::setStatus(const CircleImage::Status &stus)
{
    m_status = stus;
    emit statusChanged (m_status);
}

bool CircleImage::cache()
{
    return m_cache;
}

void CircleImage::setCache(bool cache)
{
    m_cache = cache;
    emit cacheChanged (cache);
}

void CircleImage::downloadFile(const QUrl &url)
{
    if (url.isEmpty ()) {
        this->setStatus (Status::Error);
        return;
    }

    QString path = m_settings->musicImageCachePath ();
    QString hash = Utility::calculateHash (path + url.toString ());
    path = QString("%1/%2").arg (path).arg (hash);

    ///
    /// file exist in local cache path
    ///
    if (m_cache) { //using image cache
        if (QFile::exists (path)) {
            if (fillImage (path)) { //we have the saved image data, try to load it
                this->update ();
                return;
            } else { //load image data failed, remove it and download from network
                QFile::remove (path);
                this->setStatus (Status::Error);
            }
        }

        // constructe a new file to save image data
        if (m_file) {
            m_file->close ();
            m_file->remove ();
            delete  m_file;
            m_file = 0;
        }
        m_file = new QFile(path);
        if (!m_file->open (QIODevice::WriteOnly)) {
            delete m_file;
            m_file = 0;
        }
    } else {
        if (m_file) {
            m_file->close ();
            delete m_file;
            m_file = 0;
        }
    }

    ///
    /// download from network
    ///
    if (!m_networkAccessManager)
        m_networkAccessManager = new QNetworkAccessManager(this);

    if (m_reply) {
        m_reply->abort ();
        m_requestAborted = true;
    }
    if (!m_byteArray.isEmpty ())
        m_byteArray.clear ();

    QNetworkRequest request(url);
    m_reply = m_networkAccessManager->get (request);
    if (m_reply) {
        connect (m_reply, &QNetworkReply::finished,
                 this, &CircleImage::httpFinished);
        connect (m_reply, &QNetworkReply::readyRead,
                 this, &CircleImage::httpReadyRead);
    }
}

bool CircleImage::fillImage(const QString &imagePath)
{
    if (imagePath.isEmpty ()) {
        this->setStatus (Status::Error);
        return false;
    }
    bool ret = false;

    m_lock.lock ();
    if (m_image) {
        delete m_image;
        m_image = 0;
    }
    m_image = new QImage();
    if (m_image->load (imagePath)) {
        ret = true;
    } else {
        if (m_image) {
            delete m_image;
            m_image = 0;
        }
        ret = false;
    }
    m_lock.unlock ();
    return ret;
}

bool CircleImage::fillImage(const QByteArray &qba)
{
    if (qba.isEmpty ()) {
        this->setStatus (Status::Error);
        return false;
    }
    bool ret = false;
    m_lock.lock ();
    if (m_image) {
        delete m_image;
        m_image = 0;
    }
    m_image = new QImage();
    if (m_image->loadFromData (qba)) {
        ret = true;
    } else {
        if (m_image) {
            delete m_image;
            m_image = 0;
        }
        ret = false;
    }
    m_lock.unlock ();
    return ret;
}

void CircleImage::httpFinished()
{
    if (m_requestAborted) {
        m_reply->deleteLater ();
        m_reply = 0;
        if (m_file) {
            m_file->close ();
            m_file->remove ();
            delete  m_file;
            m_file = 0;
        }
        if (!m_byteArray.isEmpty ())
            m_byteArray.clear ();
        return;
    }

    if (m_file) { // if cache set to false, mFile will be 0
        m_file->flush ();
        m_file->close ();
    }

    if (m_reply->error () != QNetworkReply::NetworkError::NoError) {
        qDebug()<<__FUNCTION__<<" download with error "<<m_reply->errorString ();
        if (m_file) {
            m_file->remove ();
        }
        m_byteArray.clear ();
    }
    m_reply->deleteLater ();
    m_reply = 0;
    if (m_file) {
        if (fillImage (m_file->fileName ())) {
            this->update ();
        } else {
            this->setStatus (Status::Error);
        }
        delete m_file;
        m_file = 0;
        return;
    }
    if (fillImage (m_byteArray))
        this->update ();
    else
        this->setStatus (Status::Error);
}

void CircleImage::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (m_file) {
        m_file->write (m_reply->readAll ());
    } else { // no local cache
        m_byteArray.append (m_reply->readAll ());
    }
}

} //QmlPlugin
} //PhoenixPlayer
