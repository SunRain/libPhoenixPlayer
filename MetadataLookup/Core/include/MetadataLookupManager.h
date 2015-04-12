#ifndef METADATALOOKUPMANAGER_H
#define METADATALOOKUPMANAGER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QPointer>
#include <QMutex>
#include "MetadataLookup/IMetadataLookup.h"
#include "SongMetaData.h"

namespace PhoenixPlayer {

class PluginLoader;
namespace MetadataLookup {

class MetadataLookupManager : public QObject
{
    Q_OBJECT
public:
    explicit MetadataLookupManager(QObject *parent = 0);
    virtual ~MetadataLookupManager();

    void lookup(SongMetaData *data, IMetadataLookup::LookupType type);
//    void setPluginLoader(PluginLoader *loader);

signals:
    void lookupSucceed(QString songHash,
                       QByteArray result,
                       IMetadataLookup::LookupType type);
    void lookupFailed(QString songHash, IMetadataLookup::LookupType type);
    void queueFinished();
public slots:

protected:
    struct WorkNode {
        SongMetaData *data;
        IMetadataLookup::LookupType type;
        bool operator == (const WorkNode &other) const {
            return (this->data->getMeta (Common::E_Hash).toString () == other.data->getMeta (Common::E_Hash).toString ())
                    && (this->type == other.type);
        }
    };

//private slots:
//    void slotDoLookup();
private:
    void setLookupConnection(/*IMetadataLookup *lookup*/);
//    void resetLookupConnection();
    void nextLookupPlugin();
    void initPluginObject();
    void initPluginNames();
    void processNext();
    void doLookup();
    void emitFinish();
    void doLookupSucceed(const QByteArray &result);
private:
//    QPointer<PluginLoader> mPluginLoader;
    PluginLoader *mPluginLoader;
    //QList<ILyricsLookup *> mPluginList;
//    QPointer<IMetadataLookup> mLookup;
    IMetadataLookup *mLookup;
//    IMetadataLookup *mBackupLookup;
//    QString mPreConnection;
//    QStringList mPluginNameList;
    QStringList mConnectList;
//    SongMetaData *mSongMeta;
    int mCurrentIndex;
//    IMetadataLookup::LookupType mCurLookupType;
    QList<WorkNode> mWorkQueue;
    QList<IMetadataLookup *>mPluginList;
    WorkNode mCurrentNode;
    QMutex mMutex;
    bool mLookupStarted;
};


} //MetadataLookup
} //PhoenixPlayer

#endif // METADATALOOKUPMANAGER_H
