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

#include "Settings.h"
#include "Util.h"

namespace PhoenixPlayer {
namespace QmlPlugin {

CircleImage::CircleImage(QQuickPaintedItem *parent)
    : QQuickPaintedItem(parent)
{
    mSource = QUrl();
    mStatus = Status::Null;
    mNetworkAccessManager = 0;
    mReply = 0;
    mImage = 0;
    mFile = 0;
    mRequestAborted = false;
    mCache = true;
    mByteArray = QByteArray();
#ifdef SAILFISH_OS
    mSettings = Settings::instance ();
#endif
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
    if (mImage == 0 || mImage->isNull ()) {
        qDebug()<<__FUNCTION__<<" Image is null";
        this->setStatus (Status::Error);
        return;
    }

//    this->setStatus (Status::Loading);

    //取得一个圆的最短长度
    int width = mImage->width();
    int targetWidth = this->width();
    int height = mImage->height();
    int targetHeight = this->height();
    QImage img = mImage->scaled(targetWidth, targetHeight, Qt::KeepAspectRatioByExpanding);

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
    return mSource;
}

void CircleImage::setSource(const QUrl &source)
{
//    qDebug()<<__FUNCTION__<<" source set to "<<source
//              <<" msource is "<<mSource;

    if (mSource == source) {
        return;
    }
    mSource = source;
    if (mSource.isEmpty() || !mSource.isValid()) {
        this->setStatus (Status::Error);
        return;
    }
    QString str = mSource.toLocalFile();
    if (str.isEmpty())
        str = mSource.toString();
    if (str.isEmpty()) {
        this->setStatus (Status::Error);
        return;
    }

    this->setStatus (Status::Loading);

    if (str.startsWith (("http"))) { //url
        downloadFile (mSource);
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
    return mStatus;
}

void CircleImage::setStatus(const CircleImage::Status &stus)
{
    mStatus = stus;
    emit statusChanged (mStatus);
}

bool CircleImage::cache()
{
    return mCache;
}

void CircleImage::setCache(bool cache)
{
    mCache = cache;
    emit cacheChanged (cache);
}

void CircleImage::downloadFile(const QUrl &url)
{
    qDebug()<<__FUNCTION__;

    if (url.isEmpty ()) {
        this->setStatus (Status::Error);
        return;
    }

    QString path = mSettings->getMusicImageCachePath ();
    QString hash = Util::calculateHash (path + url.toString ());
    path = QString("%1/%2").arg (path).arg (hash);

    ///
    /// file exist in local cache path
    ///
    if (mCache) { //using image cache
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
        if (mFile) {
            mFile->close ();
            mFile->remove ();
            delete  mFile;
            mFile = 0;
        }
        mFile = new QFile(path);
        if (!mFile->open (QIODevice::WriteOnly)) {
            delete mFile;
            mFile = 0;
        }
    } else {
        if (mFile) {
            mFile->close ();
            delete mFile;
            mFile = 0;
        }
    }

    ///
    /// download from network
    ///
    if (!mNetworkAccessManager)
        mNetworkAccessManager = new QNetworkAccessManager(this);

    if (mReply) {
        mReply->abort ();
        mRequestAborted = true;
    }
    if (!mByteArray.isEmpty ())
        mByteArray.clear ();

    QNetworkRequest request(url);
    mReply = mNetworkAccessManager->get (request);
    if (mReply) {
        connect (mReply, &QNetworkReply::finished,
                 this, &CircleImage::httpFinished);
        connect (mReply, &QNetworkReply::readyRead,
                 this, &CircleImage::httpReadyRead);
    }
}

bool CircleImage::fillImage(const QString &imagePath)
{
    qDebug()<<__FUNCTION__;

    if (imagePath.isEmpty ()) {
        this->setStatus (Status::Error);
        return false;
    }
    bool ret = false;

    mLock.lock ();
    if (mImage) {
        delete mImage;
        mImage = 0;
    }
    mImage = new QImage();
    if (mImage->load (imagePath)) {
        ret = true;
    } else {
        if (mImage) {
            delete mImage;
            mImage = 0;
        }
        ret = false;
    }
    mLock.unlock ();
    return ret;
}

bool CircleImage::fillImage(const QByteArray &qba)
{
    if (qba.isEmpty ()) {
        this->setStatus (Status::Error);
        return false;
    }
    bool ret = false;
    mLock.lock ();
    if (mImage) {
        delete mImage;
        mImage = 0;
    }
    mImage = new QImage();
    if (mImage->loadFromData (qba)) {
        ret = true;
    } else {
        if (mImage) {
            delete mImage;
            mImage = 0;
        }
        ret = false;
    }
    mLock.unlock ();
    return ret;
}

void CircleImage::httpFinished()
{
    if (mRequestAborted) {
        mReply->deleteLater ();
        mReply = 0;
        if (mFile) {
            mFile->close ();
            mFile->remove ();
            delete  mFile;
            mFile = 0;
        }
        if (!mByteArray.isEmpty ())
            mByteArray.clear ();
        return;
    }

    if (mFile) { // if cache set to false, mFile will be 0
        mFile->flush ();
        mFile->close ();
    }

    if (mReply->error () != QNetworkReply::NetworkError::NoError) {
        qDebug()<<__FUNCTION__<<" download with error "<<mReply->errorString ();
        if (mFile) {
            mFile->remove ();
        }
        mByteArray.clear ();
    }
    mReply->deleteLater ();
    mReply = 0;
    if (mFile) {
        if (fillImage (mFile->fileName ())) {
            this->update ();
        } else {
            this->setStatus (Status::Error);
        }
        delete mFile;
        mFile = 0;
        return;
    }
    if (fillImage (mByteArray))
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
    if (mFile) {
        mFile->write (mReply->readAll ());
    } else { // no local cache
        mByteArray.append (mReply->readAll ());
    }
}

} //QmlPlugin
} //PhoenixPlayer
