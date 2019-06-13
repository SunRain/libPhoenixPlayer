#ifndef METADATALOOKUPMGR_H
#define METADATALOOKUPMGR_H

#include <QObject>
#include <QMutex>
#include <QThread>

#include "libphoenixplayer_global.h"
#include "PPCommon.h"
#include "AudioMetaObject.h"
#include "MetadataLookup/IMetadataLookup.h"

class QTimer;
namespace PhoenixPlayer {
class PluginLoader;
class PluginHost;
class PPSettings;
class PPUtility;
class AudioMetaObject;

namespace MetadataLookup {

class MetadataLookupHost;
class LIBPHOENIXPLAYER_EXPORT MetadataLookupMgr : public QThread
{
    Q_OBJECT
public:
    explicit MetadataLookupMgr(QObject *parent = Q_NULLPTR);
    virtual ~MetadataLookupMgr();
    void lookup(const AudioMetaObject &data, IMetadataLookup::LookupType type);

protected:
    struct WorkNode {
        AudioMetaObject data;
        IMetadataLookup::LookupType type;
        bool operator == (const WorkNode &other) const {
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
    PluginLoader        *m_pluginLoader;
    PPSettings          *m_settings;
    PPUtility           *m_util;

    QList<WorkNode>     m_workQueue;
    WorkNode            m_currentWork;

    QList<HostNode>     m_hostList;
    HostNode            m_currentHost;

    QMutex      m_mutex;
    QMutex      m_lookupMutex;
    bool        m_finish;
//    bool        m_doInternalLoop;
    bool        m_useNextHost;
};

} //MetadataLookup
} //PhoenixPlayer

#endif // METADATALOOKUPMGR_H
