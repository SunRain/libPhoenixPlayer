#include <QCryptographicHash>

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
    return QString (QCryptographicHash::hash (str.toLocal8Bit (), QCryptographicHash::Md5));
}

} //PhoenixPlayer
