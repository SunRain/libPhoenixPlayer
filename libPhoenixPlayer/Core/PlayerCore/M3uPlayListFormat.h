#ifndef M3UPLAYLIST_H
#define M3UPLAYLIST_H

#include <QObject>

#include "libphoenixplayer_global.h"
#include "PlayListFormat.h"

namespace PhoenixPlayer {
//class AudioMetaObject;

class M3uPlayListFormat : public PlayListFormat
{
    Q_OBJECT
public:
    explicit M3uPlayListFormat(QObject *parent = 0);

    // PlayList interface
public:
    QString extension() const override;
    QStringList fileList(const QString &contents) override;
    QString format(const AudioMetaList &list) override;
};
} //PhoenixPlayer
#endif // M3UPLAYLIST_H
