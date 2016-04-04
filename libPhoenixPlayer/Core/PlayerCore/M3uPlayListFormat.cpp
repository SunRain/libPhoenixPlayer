#include "M3uPlayListFormat.h"

#include <QStringList>

#include "SongMetaData.h"

namespace PhoenixPlayer {

M3uPlayListFormat::M3uPlayListFormat(QObject *parent) :
    PlayListFormat(parent)
{
}

QString M3uPlayListFormat::extension() const
{
    return "m3u";
}

QStringList M3uPlayListFormat::fileList(const QString &contents)
{
    if (contents.isEmpty ())
        return QStringList();
    QStringList list = contents.split ("\n");
    if (list.isEmpty ())
        return QStringList();
    QStringList out;
    foreach (QString str, list) {
        str = str.trimmed ();
        if (str.startsWith ("#EXTM3U") || str.startsWith ("#EXTINF:") || str.isEmpty ())
            continue;//TODO: skip atm
        else if (str.startsWith ("#") || str.isEmpty ())
            continue;
        else
            out << str;
    }
    return out;
}

QString M3uPlayListFormat::format(const QList<AudioMetaObject *> &list)
{
    QStringList out;
    out.append ("#EXTM3U");
    foreach (AudioMetaObject *d, list) {
        //TODO add EXTINF
        out.append (d->uri ().toString ());
    }
    return out.join ("\n");
}

} //PhoenixPlayer
