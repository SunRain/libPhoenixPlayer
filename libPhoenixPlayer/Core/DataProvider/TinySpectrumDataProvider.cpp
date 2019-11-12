#include "DataProvider/TinySpectrumDataProvider.h"

#include <QCoreApplication>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "Logger.h"
#include "PluginMgr.h"
#include "DataProvider/ISpectrumGenerator.h"
#include "MusicLibrary/IMusicLibraryDAO.h"

#include "private/SingletonObjectFactory.h"
#include "private/PluginMgrInternal.h"

namespace PhoenixPlayer {
namespace DataProvider {

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
        m_usedDAO = m_pluginMgr->usedMusicLibraryDAO();
        if (PluginMetaData::isValid(m_usedDAO)) {
            m_dao = qobject_cast<MusicLibrary::IMusicLibraryDAO*>(PluginMgr::instance(m_usedDAO));
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
        if (m_dao) {
            PluginMgr::unload(m_usedDAO);
            m_dao = Q_NULLPTR;
        }
    }

    inline void stopWokerThread()
    {
        m_stop = true;
        stopGenerator();
        m_wait.wakeAll();
    }

    inline void setObject(const AudioMetaObject &obj, bool insertIntoDatabase)
    {
        m_obj = obj;
        m_insert = insertIntoDatabase;
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
                m_cancelWork = false;
                continue;
            }
            if (m_insert && m_dao) {
                m_dao->insertSpectrumData(m_obj, list);
            }
            emit generated(list);
        }
    }
signals:
    void generated(const QList<QList<qreal>> &data);

private:
    bool                                m_insert = false;
    bool                                m_stop = false;
    bool                                m_cancelWork = false;
    DataProvider::ISpectrumGenerator    *m_generator = Q_NULLPTR;
    MusicLibrary::IMusicLibraryDAO      *m_dao = Q_NULLPTR;

    QSharedPointer<PluginMgrInternal>   m_pluginMgr;

    PluginMetaData                      m_usedGenerator;
    PluginMetaData                      m_usedDAO;

    AudioMetaObject                     m_obj;

    QMutex                              m_locker;
    QWaitCondition                      m_wait;
};



TinySpectrumDataProvider::TinySpectrumDataProvider(QObject *parent)
    : QObject(parent)
{
    m_pluginMgr = SingletonObjectFactory::instance()->pluginMgrInternal();
    m_usedDAO = m_pluginMgr->usedMusicLibraryDAO();
    if (PluginMetaData::isValid(m_usedDAO)) {
        m_dao = qobject_cast<MusicLibrary::IMusicLibraryDAO*>(PluginMgr::instance(m_usedDAO));
    }
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
    if (m_dao) {
        PluginMgr::unload(m_usedDAO);
        m_dao = Q_NULLPTR;
    }
}

void TinySpectrumDataProvider::generate(const AudioMetaObject &obj, bool insertIntoDatabase)
{
    if (obj.isHashEmpty()) {
        LOG_WARNING()<<"Current object is not valid, ignore !";
        return;
    }
    if (m_dao) {
        SpectrumDataList list = m_dao->getSpectrumData(obj);
        if (!list.isEmpty()) {
            emit generated(list);
            return;
        }
    }
    if (!m_workerThread) {
        m_workerThread = new WorkerThread(this);
        connect(m_workerThread, &WorkerThread::generated,
                this, &TinySpectrumDataProvider::generated, Qt::QueuedConnection);
    }
    if (obj.isHashEmpty()) {
        LOG_WARNING()<<"AudioMetaObject is hash empty, ignore !";
        return;
    }
    m_workerThread->setObject(obj, insertIntoDatabase);
    if (!m_workerThread->isRunning()) {
        m_workerThread->start();
    }
}


} // namespace DataProvider
} // namespace PhoenixPlayer


#include "TinySpectrumDataProvider.moc"
