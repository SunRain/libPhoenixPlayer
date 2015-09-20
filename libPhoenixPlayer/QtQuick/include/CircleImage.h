#ifndef CIRCLEIMAGE_H
#define CIRCLEIMAGE_H

#include <QQuickPaintedItem>
#include <QMutex>

class QUrl;
class QNetworkAccessManager;
class QNetworkReply;
class QImage;
class QFile;
class QByteArray;
namespace PhoenixPlayer {
class Settings;
namespace QmlPlugin {
class CircleImage : public QQuickPaintedItem
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(bool cache READ cache WRITE setCache NOTIFY cacheChanged)
public:
    enum Status {
        Null = 0x0,     // - no image has been set
        Ready,          // - the image has been loaded
        Loading,        // - the image is currently being loaded
        Error           // - an error occurred while loading the image
    };

public:
    explicit CircleImage(QQuickPaintedItem *parent = 0);
    virtual ~CircleImage();

    void paint(QPainter *painter);

    QUrl source() const;
    void setSource(const QUrl &source);

    Status status() const;
    void setStatus(const Status &stus);

    bool cache();
    void setCache(bool cache);

signals:
    void sourceChanged(QUrl source);
    void statusChanged(Status status);
    void cacheChanged(bool cache);
public slots:

private:
//    emitStatusChange(const Status &st);
//    void showUrlImage(const QUrl &url);
    void downloadFile(const QUrl &url);
    bool fillImage(const QString &imagePath);
    bool fillImage (const QByteArray &qba);
    void httpFinished();
    void httpReadyRead();
private:
    QUrl m_source;
    Status m_status;
    QNetworkAccessManager *m_networkAccessManager;
    QNetworkReply *m_reply;
    QImage *m_image;
    QMutex m_lock;
    QFile *m_file;
    Settings *m_settings;
    QByteArray m_byteArray;
    bool m_requestAborted;
    bool m_cache;

};
} //QmlPlugin
} //PhoenixPlayer
#endif // CIRCLEIMAGE_H
