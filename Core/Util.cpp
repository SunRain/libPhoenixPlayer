#include <QCryptographicHash>
#include <QDebug>
#include <QByteArray>
#include <QTextCodec>
#include <QLocale>
#include <QMutex>

#include <iostream>

#include "Util.h"

namespace PhoenixPlayer {

Util::Util(QObject *parent) : QObject(parent)
{

}

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
Util *Util::instance()
{
    qDebug()<<">>>>>>>>"<<__FUNCTION__<<"<<<<<<<<<<<<<<";
    static QMutex mutex;
    static QScopedPointer<Util> scp;

    if (Q_UNLIKELY(scp.isNull())) {
        qDebug()<<">>>>>>>> statr new";
        mutex.lock();
        scp.reset(new Util(0));
        mutex.unlock();
    }
    qDebug()<<">>>>>>>> return "<<scp.data()->metaObject()->className();;
    return scp.data();
}
#endif

Util::~Util()
{

}

QString Util::calculateHash(const QString &str)
{
    return QString (QCryptographicHash::hash (str.toLocal8Bit (), QCryptographicHash::Md5).toHex ());
}

QTextCodec *Util::localeDefaultCodec()
{
    switch(QLocale::system().country())
       {
       case QLocale::China:
           return QTextCodec::codecForName("GB18030");
       case QLocale::HongKong:
           return QTextCodec::codecForName("Big5-HKSCS");
       case QLocale::Macau:
       case QLocale::Taiwan:
           return QTextCodec::codecForName("Big5");
       case QLocale::Japan:
           return QTextCodec::codecForName("Shift-JIS");
       default:
           return QTextCodec::codecForName("ISO 8859-1");
    }
}

QString Util::formateSongDuration(int time)
{
    if (time <= 0)
        return QString("00:00");

    QString str;
    if (time < 10) {
        str = QString("00:0%1").arg(time);
    } else if (time >= 10 && time < 60) {
        str = QString("00:%1").arg(time);
    } else {
        QString m = QString::number(time / 60);
        if (m.toInt() < 10)
            m = QString("0%1").arg(m);
        QString s = QString::number(time % 60);
        if (s.toInt() < 10)
            s = QString("0%1").arg(s);
        str = QString("%1:%2").arg(m).arg(s);
    }
    return str;
}

QString Util::formateFileSize(int size)
{
    if (size < 0)
        return QString("0B");
    QString str;
    if (size < 1024) { //bytes
        str = QString("%1 B").arg(size);
    } else if (size >= 1024 && size < 1024 * 1024) { //kb
        str = QString("%1 KB").arg(QString::number(((float)size)/1024, 'f', 2));
    } else { //mb
        str = QString("%1 MB").arg(QString::number(((float)size)/1024/1024, 'f', 2));
    }
    return str;
}

} //PhoenixPlayer
