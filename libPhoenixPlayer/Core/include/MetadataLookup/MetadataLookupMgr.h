#ifndef METADATALOOKUPMGR_H
#define METADATALOOKUPMGR_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QByteArray>
#include <QSharedPointer>

#include "libphoenixplayer_global.h"
//#include "PPCommon.h"
#include "AudioMetaObject.h"
#include "PluginMgr.h"
#include "DataProvider/IMetadataLookup.h"

class QTimer;
namespace PhoenixPlayer {
//    class PluginLoader;
//    class PluginHost;
//    class PPSettings;
    class PPUtility;
    class AudioMetaObject;
//    class PluginMgrInternal;

    namespace MetadataLookup {


class LookupNode
{
    friend class MetadataLookupMgr;
    friend class LookupWoker;
public:
    LookupNode() { }

    LookupNode(const LookupNode &other)
    {
        this->ret = other.ret;
        this->data = other.data;
        this->plugin = other.plugin;
        this->reqType = other.reqType;
        this->errMsg = other.errMsg;
        this->useMultiPlugin = other.useMultiPlugin;
    }

    bool operator == (const LookupNode &other) = delete;
    bool operator != (const LookupNode &other) = delete;

    LookupNode &operator = (const LookupNode &other)
    {
        this->ret = other.ret;
        this->data = other.data;
        this->plugin = other.plugin;
        this->reqType = other.reqType;
        this->errMsg = other.errMsg;
        this->useMultiPlugin = other.useMultiPlugin;
        return *this;
    }

    inline AudioMetaObject input() const
    {
        return data;
    }

    inline PluginMetaData usedPlugin() const
    {
        return plugin;
    }

    inline DataProvider::IDataProvider::SupportedType type() const
    {
        return reqType;
    }

    inline QByteArray output() const
    {
        return ret;
    }
    inline QString errorMsg() const
    {
        return errMsg;
    }
protected:
    bool                    useMultiPlugin = false;
    QByteArray              ret;
    QString                 errMsg;
    PluginMetaData          plugin;
    AudioMetaObject         data;
    DataProvider::IDataProvider::SupportedType reqType = DataProvider::IDataProvider::SupportUndefined;
};

class LookupWoker;
//TODO use thread pool later
class LIBPHOENIXPLAYER_EXPORT MetadataLookupMgr : public QObject
{
    Q_OBJECT
    friend class LookupWoker;
public:
    explicit MetadataLookupMgr(QObject *parent = Q_NULLPTR);
    virtual ~MetadataLookupMgr() override;

    /*!
     * \brief lookup
     * \param data
     * \param type
     * \param useMultiPlugin True to use all supported plugin if plugin is enabled).
     */
    void lookup(const AudioMetaObject &data, DataProvider::IDataProvider::SupportedType type, bool useMultiPlugin = false);

protected:
    /*!
     * \brief dequeue
     * Use mutex to take one lookup node from work list
     * \return Empty node if queue is empty
     */
    LookupNode dequeue();
//    inline QMutex *mutex()
//    {
//        return &m_mutex;
//    }
//    struct WorkNode {
//        AudioMetaObject data;
//        DataProvider::IDataProvider::SupportedType type;
//        bool operator == (const WorkNode &other) const {
//            return data.hash () == other.data.hash ()
//                    && type == other.type;
//        }
//    };
//    struct HostNode {
//        IMetadataLookup **lookup;
//        MetadataLookupHost *host;
//    };

//    // QThread interface
//protected:
//    virtual void run() Q_DECL_OVERRIDE;

signals:
//    void lookupSucceed(const QString &songHash,
//                       const QByteArray &result,
//                       const IMetadataLookup::LookupType &type);
//    void lookupSucceed(const AudioMetaObject &data, const IMetadataLookup::LookupType &type);
////    void lookupFailed(const QString &songHash, const IMetadataLookup::LookupType &type);
//    void lookupFailed(const AudioMetaObject &data, const IMetadataLookup::LookupType &type);
    void lookupOne(const LookupNode &node);
    void queueFinished();

private slots:
//    void nextLookupPlugin();
//    void doLookupSucceed(const QByteArray &result);
//    void doLookupFailed();
//    void destructor();
private:
    //在destructorState里面使用一个事件循环，来判断是否处在卸载插件的状态
//    bool destructorState();
//    void initPluginObject();
//    void processNext();
//    void doLookup();
    void emitFinish();
private:
    LookupWoker                         *m_worker = Q_NULLPTR;
//    PPUtility           *m_util = Q_NULLPTR;

    QList<LookupNode>                   m_workQueue;
//    LookupNode                          m_curNode;
//    PluginLoader        *m_pluginLoader;
//    PPSettings          *m_settings;

//    QList<WorkNode>     m_workQueue;
//    WorkNode            m_currentWork;

//    QList<HostNode>     m_hostList;
//    HostNode            m_currentHost;

    QMutex      m_mutex;
//    QMutex      m_lookupMutex;
//    bool        m_finish;
////    bool        m_doInternalLoop;
//    bool        m_useNextHost;
};

} //MetadataLookup
} //PhoenixPlayer

#endif // METADATALOOKUPMGR_H
