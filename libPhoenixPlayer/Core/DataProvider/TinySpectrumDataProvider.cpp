#include "DataProvider/TinySpectrumDataProvider.h"

#include <QCoreApplication>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <qcompilerdetection.h>

#include "Logger.h"
#include "PluginMgr.h"
#include "DataProvider/ISpectrumGenerator.h"
#include "MusicLibrary/IMusicLibraryDAO.h"

#include "private/SingletonObjectFactory.h"
#include "private/PluginMgrInternal.h"

namespace PhoenixPlayer {
namespace DataProvider {

static QString s_cacheDir;
inline QString cacheDir()
{
    if (Q_UNLIKELY(s_cacheDir.isEmpty())) {
        QString cache = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        if (cache.endsWith("/")) {
            s_cacheDir = QString("%1SpekData").arg(cache);
        } else {
            s_cacheDir = QString("%1/SpekData").arg(cache);
        }
        QDir dir(s_cacheDir);
        if (!dir.exists()) {
            dir.mkpath(s_cacheDir);
        }
    }
    return s_cacheDir;
}

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread(QObject *parent = Q_NULLPTR)
        : QThread(parent)
    {
        m_pluginMgr = SingletonObjectFactory::instance()->pluginMgrInternal();

        QList<PluginMetaData> list = m_pluginMgr->pluginMetaDataList(DataProvider::IDataProvider::SupportSpectrumGenerator);
        if (!list.isEmpty()) {
            //TODO only use first one enabled plugin atm
            foreach (const auto &it, list) {
                if (it.enabled) {
                    m_usedGenerator = it;
                    m_generator = qobject_cast<DataProvider::ISpectrumGenerator*>(PluginMgr::instance(it));
                    if (m_generator) {
                        break;
                    }
                }
            }
        }
    }
    virtual ~WorkerThread() override
    {
        if (this->isRunning()) {
            stopWokerThread();
            m_wait.wakeAll();
            this->quit();
            this->wait(500);
        }
        if (m_generator) {
            PluginMgr::unload(m_usedGenerator);
            m_generator = Q_NULLPTR;
        }
    }

    inline void stopWokerThread()
    {
        m_stop = true;
        stopGenerator();
        m_wait.wakeAll();
    }

    inline void setObject(const AudioMetaObject &obj)
    {
        m_obj = obj;
        if (!m_obj.isHashEmpty() && this->isRunning()) {
            stopGenerator();
        }
        m_wait.wakeAll();
    }

protected:
    inline void stopGenerator()
    {
        m_cancelWork = true;
        if (m_generator) {
            m_generator->stop();
        }
    }
    // QThread interface
protected:
    void run() Q_DECL_OVERRIDE
    {
        if (!m_generator) {
            LOG_ERROR()<<"Can't find valid SpectrumGenerator plugin !!";
            return;
        }
        while (true) {
            if (m_stop) {
                break;
            }
            m_locker.lock();
            if (m_obj.isHashEmpty() && !m_stop) {
                LOG_DEBUG()<<"Current obj is hash empty, wait";
                m_wait.wait(&m_locker);
            }
            m_locker.unlock();

            if (m_stop) {
                break;
            }

            m_cancelWork = false;
            SpectrumDataList list = m_generator->generate(m_obj);
            if (m_cancelWork) {
                qDebug()<<"request cancel work";
                m_cancelWork = false;
                m_obj = AudioMetaObject();
                continue;
            }

            QFile file(QString("%1/%2.spek").arg(cacheDir()).arg(m_obj.hash()));
            if (file.exists()) {
                file.remove();
            }
            file.open(QIODevice::WriteOnly);
            QDataStream ds(&file);
            ds << list;
            file.flush();
            file.close();

            emit generated(m_obj, list);
            m_obj = AudioMetaObject();
        }
    }
signals:
    void generated(const AudioMetaObject &obj, const QList<QList<qreal>> &data);

private:
    bool                                m_stop = false;
    bool                                m_cancelWork = false;
    DataProvider::ISpectrumGenerator    *m_generator = Q_NULLPTR;
    QSharedPointer<PluginMgrInternal>   m_pluginMgr;

    PluginMetaData                      m_usedGenerator;
    AudioMetaObject                     m_obj;

    QMutex                              m_locker;
    QWaitCondition                      m_wait;
};



TinySpectrumDataProvider::TinySpectrumDataProvider(QObject *parent)
    : QObject(parent)
{

}

TinySpectrumDataProvider::~TinySpectrumDataProvider()
{
    if (m_workerThread && m_workerThread->isRunning()) {
        m_workerThread->stopWokerThread();
        m_workerThread->quit();
        m_workerThread->wait(1000);
        m_workerThread->deleteLater();
        m_workerThread = Q_NULLPTR;
    }
}

void TinySpectrumDataProvider::generate(const AudioMetaObject &obj)
{
    if (obj.isHashEmpty()) {
        LOG_WARNING()<<"Current object is not valid, ignore !";
        return;
    }
    QFile file(QString("%1/%2.spek").arg(cacheDir()).arg(obj.hash()));
    if (file.exists()) {
        file.open(QIODevice::ReadOnly);
        QDataStream ds(&file);
        SpectrumDataList list;
        ds >> list;
        file.close();
        emit generated(obj, list);
        return;
    }

    if (!m_workerThread) {
        m_workerThread = new WorkerThread(this);
        connect(m_workerThread, &WorkerThread::generated,
                this, &TinySpectrumDataProvider::generated,
                Qt::QueuedConnection);
    }
    if (obj.isHashEmpty()) {
        LOG_WARNING()<<"AudioMetaObject is hash empty, ignore !";
        return;
    }
    m_workerThread->setObject(obj);
    if (!m_workerThread->isRunning()) {
        m_workerThread->start();
    }
}


} // namespace DataProvider
} // namespace PhoenixPlayer


#include "TinySpectrumDataProvider.moc"
