#ifndef CIRCLEIMAGE_H
#define CIRCLEIMAGE_H

#include <QQuickPaintedItem>

class QUrl;
//class QPixmap;
//class QImage;
//class QScopedPointer;
namespace PhoenixPlayer {
namespace QmlPlugin {
class CircleImage : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
public:
    explicit CircleImage(QQuickPaintedItem *parent = 0);
    virtual ~CircleImage();

    void paint(QPainter *painter);

    QUrl source() const;
signals:
    void sourceChanged(QUrl source);
public slots:
    void setSource(const QUrl &source);
private:
    QUrl mSource;
//    QScopedPointer<QPixmap> mPixmap;
//    QScopedPointer<QImage> mImage;

};
} //QmlPlugin
} //PhoenixPlayer
#endif // CIRCLEIMAGE_H
