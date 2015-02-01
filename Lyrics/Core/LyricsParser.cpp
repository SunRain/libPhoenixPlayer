#include <QDate>
#include <QRegExp>
#include <QTextStream>
#include <QDebug>

#include "LyricsParser.h"

namespace PhoenixPlayer {
namespace Lyrics {

QLyricsList LyricParser::parserLyrics(const QString &lyricsStr)
{
    if (lyricsStr.isEmpty ())
        return QLyricsList();

    qDebug()<<"LyricParser start parser";

    QRegExp timeExp;
    timeExp.setPatternSyntax(QRegExp::RegExp);
    timeExp.setCaseSensitivity(Qt::CaseSensitive);
    timeExp.setPattern("\\[([0-9]{2}):([0-9]{2})\\.([0-9]{2})\\]");

    QString str = lyricsStr;
    QTextStream stream(&str, QIODevice::ReadOnly);

    QLyricsList result;
    while (!stream.atEnd()) {
        QString line = stream.readLine();

        qDebug()<<"readline "<<line;

        int ret = timeExp.indexIn(line);
        QList<QTime> ticks;
        int lastindex = 0;
        while (ret >= 0) {
            QStringList tstr = timeExp.capturedTexts();
            QTime time(0, tstr[1].toInt(), tstr[2].toInt(), tstr[3].toInt());
            ticks.append(time);
            lastindex = ret + timeExp.matchedLength();
            ret = timeExp.indexIn(line, lastindex);
        }
        QString lyricstr = line.right(line.size() - lastindex);
        for (const QTime& t : ticks) {
            QLyrics lyrics;
            lyrics.time = t;
            lyrics.lyrics = lyricstr;
            qDebug()<<"Try to append lyrics line [time] "<<t.toString ()
                   <<" [text] " <<lyricstr;

            result.append(lyrics);
        }
    }
    qStableSort(result.begin(),
              result.end(),
              [] (const QLyrics& a, const QLyrics& b) -> bool {
        return a.time < b.time;
    });

    return result;
}

LyricParser::LyricParser(QObject *parent) : QObject(parent)
{

}

LyricParser::~LyricParser()
{

}


} //Lyrics
} //PhoenixPlayer
