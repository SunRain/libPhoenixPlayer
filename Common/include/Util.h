#ifndef UTIL_H
#define UTIL_H

#include <QObject>
#include <QTextCodec>

namespace PhoenixPlayer {

class Util : public QObject
{
    Q_OBJECT
public:
//    static Util *getInstance();
#ifdef SAILFISH_OS
    static Util *instance();
#endif
    virtual ~Util();

    static QString calculateHash(const QString &str);

    static QTextCodec *localeDefaultCodec();

    Q_INVOKABLE static QString formateSongDuration(int time);
    Q_INVOKABLE static QString formateFileSize(int size);
signals:

public slots:

private:
    explicit Util(QObject *parent = 0);
};
} //PhoenixPlayer
#endif // UTIL_H
