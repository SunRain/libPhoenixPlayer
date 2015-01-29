#ifndef UTIL_H
#define UTIL_H

#include <QObject>
#include <QTextCodec>

namespace PhoenixPlayer {

class Util : public QObject
{
    Q_OBJECT
public:
    static Util *getInstance();
    virtual ~Util();

    static QString calculateHash(const QString &str);

    static QTextCodec *localeDefaultCodec();
signals:

public slots:

private:
    explicit Util(QObject *parent = 0);
};
} //PhoenixPlayer
#endif // UTIL_H
