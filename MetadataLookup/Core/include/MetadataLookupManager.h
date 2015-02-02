#ifndef METADATALOOKUPMANAGER_H
#define METADATALOOKUPMANAGER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QPointer>

namespace PhoenixPlayer {

class SongMetaData;
class PluginLoader;
namespace MetadataLookup {

class IMetadataLookup;
class MetadataLookupManager : public QObject
{
    Q_OBJECT
public:
    explicit MetadataLookupManager(QObject *parent = 0);
    virtual ~MetadataLookupManager();

    void lookup(SongMetaData *data);
    void setPluginLoader(PluginLoader *loader);

signals:
    void lookupSucceed(QString songHash, QString lyricsStr);
    void lookupFailed();
public slots:

private:
    QPointer<PluginLoader> mPluginLoader;
    //QList<ILyricsLookup *> mPluginList;
    IMetadataLookup *mLookup;
    QStringList mPluginNameList;
    SongMetaData *mSongMeta;
    int mCurrentIndex;
//    bool mLyricsFound;
};


} //MetadataLookup
} //PhoenixPlayer

#endif // METADATALOOKUPMANAGER_H
