#ifndef M3UPLAYLIST_H
#define M3UPLAYLIST_H

#include <QObject>

#include "PlayListFormat.h"

namespace PhoenixPlayer {
class SongMetaData;

class M3uPlayListFormat : public PlayListFormat
{
    Q_OBJECT
public:
    explicit M3uPlayListFormat(QObject *parent = 0);

    // PlayList interface
public:
    QString extension() const;
    QStringList fileList(const QString &contents);
    QString format(const QList<SongMetaData *> &list);
};
} //PhoenixPlayer
#endif // M3UPLAYLIST_H
