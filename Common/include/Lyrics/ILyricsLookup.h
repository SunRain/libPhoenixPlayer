#ifndef ILYRICSLOOKUP
#define ILYRICSLOOKUP

#include <QObject>

namespace PhoenixPlayer{
class SongMetaData;
namespace Lyrics {

class ILyricsLookup : public QObject {
    Q_OBJECT
public:
    explicit ILyricsLookup(QObject *parent = 0) : QObject(parent) {}

    virtual QString getPluginName() = 0;
    virtual QString getPluginVersion() = 0;

    virtual void lookup(SongMetaData *meta) = 0;

signals:
    void lookupFailed();
    void lookupSucceed(QString lyricsString);
};

} //Lyrics
} //PhoenixPlayer

Q_DECLARE_INTERFACE(PhoenixPlayer::Lyrics::ILyricsLookup, "PhoenixPlayer.Lyrics.ILyricsLookup/1.0")

#endif // ILYRICSLOOKUP

