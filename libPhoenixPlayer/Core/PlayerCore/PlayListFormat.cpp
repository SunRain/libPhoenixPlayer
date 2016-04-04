#include "PlayListFormat.h"

#include <QStringList>

namespace PhoenixPlayer {
PlayListFormat::PlayListFormat(QObject *parent) :
    QObject(parent)
{
}

QString PlayListFormat::extension() const
{
    return QString();
}

QStringList PlayListFormat::fileList(const QString &contents)
{
    return QStringList();
}

QString PlayListFormat::format(const QList<AudioMetaObject *> &list)
{
    return QString();
}

//QString PlayList::encode(const QStringList &fileList)
//{
//    return QString();
//}
} //namespace PhoenixPlayer {
