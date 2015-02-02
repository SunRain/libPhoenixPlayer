#ifndef METADATALOOKUPMANAGER_H
#define METADATALOOKUPMANAGER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QPointer>
#include "MetadataLookup/IMetadataLookup.h"
#include "SongMetaData.h"

class QDebug;
namespace PhoenixPlayer {

class PluginLoader;
namespace MetadataLookup {

class MetadataLookupManager : public QObject
{
    Q_OBJECT
public:
    explicit MetadataLookupManager(QObject *parent = 0);
    virtual ~MetadataLookupManager();

    ///
    /// \brief reset 将
    ///
    void reset();
    void lookup(SongMetaData *data, IMetadataLookup::LookupType type);
    void setPluginLoader(PluginLoader *loader);

signals:
    void lookupSucceed(QString songHash,
                       QByteArray result,
                       IMetadataLookup::LookupType type);
    void lookupFailed();
public slots:

protected:
    struct WorkNode {
        SongMetaData *data;
        IMetadataLookup::LookupType type;
    };

private:
    void nextLookupPlugin();
    void initPlugins();
    void processNext();
private:
    QPointer<PluginLoader> mPluginLoader;
    //QList<ILyricsLookup *> mPluginList;
    IMetadataLookup *mLookup;
    IMetadataLookup *mBackupLookup;
    QStringList mPluginNameList;
//    SongMetaData *mSongMeta;
    int mCurrentIndex;
//    IMetadataLookup::LookupType mCurLookupType;
    QList<WorkNode> mWorkQueue;
    WorkNode mCurrentNode;
};


} //MetadataLookup
} //PhoenixPlayer

#endif // METADATALOOKUPMANAGER_H
