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

class IPlayListDAO;
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
    QThread *mThread;
    TagParserManager *mTagParserManager;
    QList<PhoenixPlayer::SongMetaData *> *mMetaDataList;
    PluginLoader *mPluginLoader;
    IPlayListDAO *mDao;
    QMutex mListLocker;
    QMutex mCommitLocker;
};


} //MusicLibrary
} //PhoenixPlayer
#endif // ASYNCTAGPARSERMGRWRAPPER_H
