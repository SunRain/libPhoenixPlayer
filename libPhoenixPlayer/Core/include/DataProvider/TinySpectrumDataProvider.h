#ifndef TINYSPECTRUMDATAPROVIDER_H
#define TINYSPECTRUMDATAPROVIDER_H

#include <QObject>
#include <QSharedPointer>

#include "AudioMetaObject.h"
#include "PluginMgr.h"

namespace PhoenixPlayer {

    class PluginMgrInternal;

    namespace MusicLibrary {
        class IMusicLibraryDAO;
    }

    namespace DataProvider {

        class WorkerThread;
/*!
 * \brief The TinySpectrumDataProvider class is a 'Single-thread' spectrum generate class.
 * This means it support generate only ONE file at ONE time, if it is doing generate task
 * and you call generate another file again, it will cancel previous task and start the new one,
 * only last task data will be emmited if task finished
 */
class TinySpectrumDataProvider : public QObject
{
    Q_OBJECT
public:
    explicit TinySpectrumDataProvider(QObject *parent = Q_NULLPTR);
    virtual ~TinySpectrumDataProvider();

    void generate(const AudioMetaObject &obj, bool insertIntoDatabase = true);

signals:
    void generated(const QList<QList<qreal>> &data);

private:
    WorkerThread                        *m_workerThread = Q_NULLPTR;
    MusicLibrary::IMusicLibraryDAO      *m_dao = Q_NULLPTR;
    QSharedPointer<PluginMgrInternal>   m_pluginMgr;
    PluginMetaData                      m_usedDAO;
};

} // namespace DataProvider
} // namespace PhoenixPlayer


#endif // TINYSPECTRUMDATAPROVIDER_H
