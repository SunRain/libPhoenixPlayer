#ifndef UTIL_H
#define UTIL_H

#include <QObject>
#include <QTextCodec>

namespace PhoenixPlayer {

class Util : public QObject
{
    Q_OBJECT
public:
#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    static Util *instance();
#endif
    explicit Util(QObject *parent = 0);
    virtual ~Util();

    static QString calculateHash(const QString &str);

    static QTextCodec *localeDefaultCodec();

    Q_INVOKABLE static QString formateSongDuration(int time);
    Q_INVOKABLE static QString formateFileSize(int size);

signals:

public slots:
};
} //PhoenixPlayer
#endif // UTIL_H
