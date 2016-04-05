#ifndef METADATALOOKUPMGR_H
#define METADATALOOKUPMGR_H

#include <QObject>
#include <QMutex>
#include <QThread>

#include "libphoenixplayer_global.h"
#include "Common.h"
#include "AudioMetaObject.h"
#include "MetadataLookup/IMetadataLookup.h"

class QTimer;
namespace PhoenixPlayer {
class PluginLoader;
class PluginHost;
class Settings;
class Util;
class AudioMetaObject;

namespace MetadataLookup {

class MetadataLookupHost;
class LIBPHOENIXPLAYER_EXPORT MetadataLookupMgr : public QThread
{
    Q_OBJECT
public:
    explicit MetadataLookupMgr(QObject *parent = 0);
    virtual ~MetadataLookupMgr();
    void lookup(const AudioMetaObject &data, IMetadataLookup::LookupType type);

protected:
    struct WorkNode {
        AudioMetaObject data;
        IMetadataLookup::LookupType type;
        bool operator == (const WorkNode &other) const {
//            return ((*(this->data))->hash () == (*(other.data))->hash ()
//                    && (this->type == other.type));
            return data.hash () == other.data.hash ()
                    && type == other.type;
        }
    };
    struct HostNode {
        IMetadataLookup **lookup;
        MetadataLookupHost *host;
    };

    // QThread interface
protected:
    void run();

signals:
//    void lookupSucceed(const QString &songHash,
//                       const QByteArray &result,
//                       const IMetadataLookup::LookupType &type);
    void lookupSucceed(const AudioMetaObject &data, const IMetadataLookup::LookupType &type);
//    void lookupFailed(const QString &songHash, const IMetadataLookup::LookupType &type);
    void lookupFailed(const AudioMetaObject &data, const IMetadataLookup::LookupType &type);
    void queueFinished();
public slots:

private slots:
//    void nextLookupPlugin();
    void doLookupSucceed(const QByteArray &result);
    void doLookupFailed();
//    void destructor();
private:
    //在destructorState里面使用一个事件循环，来判断是否处在卸载插件的状态
//    bool destructorState();
    void initPluginObject();
//    void processNext();
//    void doLookup();
    void emitFinish();
private:
    PluginLoader *m_pluginLoader;
//    IMetadataLookup *m_currentLookup;
    Settings *m_settings;
    Util *m_util;

    QList<WorkNode> m_workQueue;
    WorkNode m_currentWork;

//    QList<IMetadataLookup *> m_lookupList;
    QList<HostNode> m_hostList;
//    QList<HostNode>::const_iterator m_currentHost;
    HostNode m_currentHost;
//    int m_currentHostIndex;
//    IMetadataLookup **m_currentLookup;

    QMutex m_mutex;
    QMutex m_lookupMutex;
    bool m_finish;
    bool m_doInternalLoop;
    bool m_useNextHost;
//    QTimer *m_destructorTimer;
//    QMutex m_workQueueLock;
//    QMutex m_startLookupLock;
//    QMutex m_destructorLock;
//    int m_currentLookupIndex;
//    bool m_lookupStarted;
//    bool m_destructorState;
};

} //MetadataLookup
} //PhoenixPlayer

#endif // METADATALOOKUPMGR_H
