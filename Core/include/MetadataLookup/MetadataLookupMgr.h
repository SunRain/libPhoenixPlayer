#ifndef METADATALOOKUPMGR_H
#define METADATALOOKUPMGR_H

#include <QObject>
#include <QMutex>

#include "Common.h"
#include "SongMetaData.h"
#include "MetadataLookup/IMetadataLookup.h"

class QTimer;
namespace PhoenixPlayer {
class PluginLoader;
class PluginHost;
class Settings;
class Util;
namespace MetadataLookup {
class MetadataLookupMgr : public QObject
{
    Q_OBJECT
public:
    explicit MetadataLookupMgr(QObject *parent = 0);
    virtual ~MetadataLookupMgr();
    void lookup(SongMetaData *data, IMetadataLookup::LookupType type);

protected:
    struct WorkNode {
        SongMetaData *data;
        IMetadataLookup::LookupType type;
        bool operator == (const WorkNode &other) const {
            return ((this->data->getMeta (Common::E_Hash).toString ()
                         == other.data->getMeta (Common::E_Hash).toString ())
                    && (this->type == other.type));
        }
    };

signals:
    void lookupSucceed(const QString &songHash,
                       const QByteArray &result,
                       const IMetadataLookup::LookupType &type);
    void lookupFailed(const QString &songHash, const IMetadataLookup::LookupType &type);
    void queueFinished();
public slots:

private slots:
    void nextLookupPlugin();
    void doLookupSucceed(const QByteArray &result);
    void doLookupFailed();
    void destructor();
private:
    //在destructorState里面使用一个事件循环，来判断是否处在卸载插件的状态
    bool destructorState();
    void initPluginObject();
    void processNext();
    void doLookup();
    void emitFinish();
private:
    PluginLoader *mPluginLoader;
    IMetadataLookup *mCurrentLookup;
    Settings *mSettings;
    Util *mUtil;

    QList<WorkNode> mWorkQueue;
    WorkNode mCurrentNode;

    QList<IMetadataLookup *> mLookupList;
//    QList<PluginHost *> mHostList;

    QTimer *mDestructorTimer;
    QMutex mWorkQueueLock;
    QMutex mStartLookupLock;
    QMutex mDestructorLock;
    int mCurrentLookupIndex;
    bool mLookupStarted;
    bool mDestructorState;
};

} //MetadataLookup
} //PhoenixPlayer

#endif // METADATALOOKUPMGR_H
