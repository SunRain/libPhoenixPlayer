#ifndef LYRICSMANAGER_H
#define LYRICSMANAGER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QPointer>

namespace PhoenixPlayer {

class SongMetaData;
class PluginLoader;
namespace Lyrics {

class ILyricsLookup;
class LyricsManager : public QObject
{
    Q_OBJECT
public:
    explicit LyricsManager(QObject *parent = 0);
    virtual ~LyricsManager();

    void lookup(SongMetaData *data);
    void setPluginLoader(PluginLoader *loader);

signals:
    void lookupSucceed(QString songHash, QString lyricsStr);
    void lookupFailed();
public slots:

private:
    QPointer<PluginLoader> mPluginLoader;
    //QList<ILyricsLookup *> mPluginList;
    ILyricsLookup *mLookup;
    QStringList mPluginNameList;
    SongMetaData *mSongMeta;
    int mCurrentIndex;
//    bool mLyricsFound;
};


} //Lyrics
} //PhoenixPlayer

#endif // LYRICSMANAGER_H
