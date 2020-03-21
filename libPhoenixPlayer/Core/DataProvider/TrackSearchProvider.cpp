#include "DataProvider/TrackSearchProvider.h"

#include <QDebug>
#include <QRunnable>

#include "PluginMgr.h"
#include "private/SingletonObjectFactory.h"
#include "private/PluginMgrInternal.h"

namespace PhoenixPlayer {
namespace DataProvider {

class Worker : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Worker(const QString &str,
                    ITrackSearch::MatchTypes type,
                    const QList<PluginMetaData> &plugins,
                    QObject *parent = Q_NULLPTR)
        : QObject(parent),
          m_queryStr(str),
          m_type(type),
          m_pluginMetaList(plugins)
    {

    }
    virtual ~Worker()
    {
        qDebug()<<" ------- ";
    }

Q_SIGNALS:
    void matched(const QString &pattern, const QList<MatchObject> &objList);

    // QRunnable interface
public:
    virtual void run() Q_DECL_OVERRIDE
    {
        QList<MatchObject> objList;

        foreach (const auto &meta, m_pluginMetaList) {
            ITrackSearch *plugin = qobject_cast<ITrackSearch*>(PluginMgr::instance(meta));
            if (!plugin) {
                continue;
            }
            QList<MatchObject> list = plugin->match(m_queryStr, m_type);
            if (!list.isEmpty()) {
                objList.append(list);
            }
        }
        Q_EMIT matched(m_queryStr, objList);
    }

private:
    QString                     m_queryStr;
    ITrackSearch::MatchTypes    m_type;
    QList<PluginMetaData>       m_pluginMetaList;
};


TrackSearchProvider::TrackSearchProvider(QObject *parent)
    : QObject(parent)
{
    m_threadPool = new QThreadPool(this);
    QSharedPointer<PluginMgrInternal> pluginMgr = SingletonObjectFactory::instance()->pluginMgrInternal();
    QList<PluginMetaData> list = pluginMgr->dataProviderList(DataProvider::IDataProvider::SupportTrackSearch);
    if (!list.isEmpty()) {
        foreach (const auto &it, list) {
            if (it.enabled) {
                if (qobject_cast<DataProvider::ITrackSearch*>(PluginMgr::instance(it))) {
                    m_pluginMetaList.append(it);
                }
            }
        }
    }
}

TrackSearchProvider::~TrackSearchProvider()
{

}

QList<PluginMetaData> TrackSearchProvider::enabledPlugins() const
{
    return m_pluginMetaList;
}

void TrackSearchProvider::search(const QString &pattern, ITrackSearch::MatchTypes type)
{
    Worker *work = new Worker(pattern, type, m_pluginMetaList);
    connect(work, &Worker::matched, this, &TrackSearchProvider::matched, Qt::QueuedConnection);
    m_threadPool->start(work);
}

void TrackSearchProvider::searchBy(const QString &pattern, ITrackSearch::MatchTypes type, const PluginMetaData &plugin)
{
    QList<PluginMetaData> list;
    list.append(plugin);

    Worker *work = new Worker(pattern, type, m_pluginMetaList);
    connect(work, &Worker::matched, this, &TrackSearchProvider::matched, Qt::QueuedConnection);
    m_threadPool->start(work);
}

QMap<QString, QList<MatchObject> > TrackSearchProvider::trim(const QList<MatchObject> &objList)
{
    QMap<QString, QList<MatchObject>> map;
    foreach (const auto &it, objList) {
        if (map.contains(it.uri())) {
            auto list = map.value(it.uri());
            list.append(it);
            map.insert(it.uri(), list);
        } else {
            QList<MatchObject> list;
            list.append(it);
            map.insert(it.uri(), list);
        }
    }
    return map;
}


} // namespace DataProvider
} // namespace PhoenixPlayer

#include "TrackSearchProvider.moc"
