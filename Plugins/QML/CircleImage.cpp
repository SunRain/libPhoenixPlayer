#include "CircleImage.h"

#include <QUrl>
#include <QScopedPointer>
#include <QPixmap>
#include <QImage>
#include <QBrush>
#include <QPainter>
#include <QDebug>

namespace PhoenixPlayer {
namespace QmlPlugin {

CircleImage::CircleImage(QQuickPaintedItem *parent)
    : QQuickPaintedItem(parent)
{
    mSource = QUrl();
    //    mPixmap = 0;
}

CircleImage::~CircleImage()
{
}

void CircleImage::paint(QPainter *painter)
{
    if (mSource.isEmpty() || !mSource.isValid())
        return;
    QString str = mSource.toLocalFile();
    if (str.isEmpty())
        str = mSource.toString();
    if (str.isEmpty())
        return;

    //remove qrc: from file uri to fix image load
    if (str.startsWith("qrc:"))
        str = str.mid(3, str.length() - 3);

    QImage image(str);
    if (image.isNull()) {
        qDebug()<<"Image is null";
        return;
    }

    //取得一个圆的最短长度
    int width = image.width();
    int targetWidth = this->width();
    int height = image.height();
    int targetHeight = this->height();
//    if (width > targetWidth || height > targetHeight) {
        image = image.scaled(targetWidth, targetHeight,
                             Qt::KeepAspectRatio);
//    }
    width = image.width();
    height = image.height();

    int radius = width > height ? height : width;

//    qDebug()<<"image width is "<<image.width()
//              << "image heigth is "<<image.height()
//                 <<" this width is "<<this->width()
//                   <<" this height is "<<this->height()
//                 <<" radius "<<radius
//                   <<" Draw x "<<(this->width() - radius)/2
//                     <<" Draw y "<<(this->height() - radius)/2;

    //make a transparent image
    QImage out(radius, radius, QImage::Format_ARGB32_Premultiplied);
    out.fill(Qt::transparent);

    QBrush b(image);
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
}

QUrl CircleImage::source() const
{
    return mSource;
}

void CircleImage::setSource(const QUrl &source)
{
    if (source.isEmpty() || !source.isValid() || mSource == source)
        return;
    mSource = source;

//    qDebug()<<"set Source file is "<<source;

    this->update();
}

} //QmlPlugin
} //PhoenixPlayer
