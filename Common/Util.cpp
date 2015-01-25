#include <QCryptographicHash>
#include <QDebug>
#include <QByteArray>
#include <QTextCodec>
#include <QLocale>

#include <iostream>

#include "Util.h"

namespace PhoenixPlayer {

Util::Util(QObject *parent) : QObject(parent)
{

}

Util *Util::getInstance()
{
    static Util u;
    return &u;
}

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

} //PhoenixPlayer
