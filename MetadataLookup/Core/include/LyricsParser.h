#ifndef LYRICSPARSER_H
#define LYRICSPARSER_H

#include <QObject>
#include <QTime>

namespace PhoenixPlayer {
namespace MetadataLookup {

struct QLyrics
{
    QTime time;
    QString lyrics;
};

typedef QList<QLyrics> QLyricsList;

class LyricParser : public QObject
{
    Q_OBJECT
public:
    //返回lyricslist, 如果失败,返回空列表
    static QLyricsList parserLyrics(const QString &lyricsStr);
private:
    explicit LyricParser(QObject *parent = 0);
    ~LyricParser();
};

} //Lyrics
} //PhoenixPlayer
#endif // LYRICSPARSER_H
