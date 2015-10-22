#ifndef LOCALMUSICSCANNERTHREAD_H
#define LOCALMUSICSCANNERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QMimeDatabase>

class QStringList;

namespace PhoenixPlayer {

class Settings;
class PluginLoader;
class SongMetaData;
namespace MusicLibrary {

class IMusicTagParser;
class IMusicLibraryDAO;
class MusicTagParserHost;
class IMusicTagParser;
class LocalMusicScannerThread : public QThread
{
    Q_OBJECT
public:
    explicit LocalMusicScannerThread(QObject *parent = 0);  
    virtual ~LocalMusicScannerThread();

    ///
    /// \brief stopLookup 停止搜索目录
    /// \return
    ///
    void stopLookup();
    void addLookupDir(const QString &dirName, bool lookupImmediately = false);
    void addLookupDirs(const QStringList &dirList, bool lookupImmediately = false);

    // QThread interface
protected:
    void run();

protected:
    void scanDir(const QString &path);
signals:

public slots:

private:
    QStringList m_pathList;
    bool m_stopLookupFlag;
    QMimeDatabase m_QMimeDatabase;
    QMutex m_mutex;

    Settings *m_settings;
    PluginLoader *m_pluginLoader;
    IMusicLibraryDAO *m_dao;
    QList<MusicTagParserHost *> m_tagHostList;
    QList<IMusicTagParser *> m_tagParserList;
};
} //MusicLibrary
} //PhoenixPlayer

#endif // LOCALMUSICSCANNERTHREAD_H
