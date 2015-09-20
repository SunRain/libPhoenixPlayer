#ifndef ASYNCTAGPARSERMGRWRAPPER_H
#define ASYNCTAGPARSERMGRWRAPPER_H

#include <QObject>
#include <QList>
#include <QMutex>

class QThread;
namespace PhoenixPlayer {

class PluginLoader;
class SongMetaData;
namespace MusicLibrary {

class IMusicLibraryDAO;
class TagParserManager;
class AsyncTagParserMgrWrapper : public QObject
{
    Q_OBJECT
public:
    explicit AsyncTagParserMgrWrapper(QObject *parent = 0);
    virtual ~AsyncTagParserMgrWrapper();
//    void addFile(const QString &path, const QString &file, qint64 size);
//    void startParser();
    void parser(QList<PhoenixPlayer::SongMetaData *> *list = 0);
    bool isRunning();
signals:
    void started();
    void finished();
public slots:

private:
    QThread *m_thread;
    TagParserManager *m_tagParserManager;
    QList<PhoenixPlayer::SongMetaData *> *m_metaDataList;
    PluginLoader *m_pluginLoader;
    IMusicLibraryDAO *m_dao;
    QMutex m_listLocker;
    QMutex m_commitLocker;
};


} //MusicLibrary
} //PhoenixPlayer
#endif // ASYNCTAGPARSERMGRWRAPPER_H
