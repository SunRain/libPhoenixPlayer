#ifndef LOCALMUSICSCANNERTHREAD_H
#define LOCALMUSICSCANNERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QMimeDatabase>

#include "libphoenixplayer_global.h"

class QStringList;

namespace PhoenixPlayer {

class PPSettings;
class PluginLoader;
class AudioMetaObject;
namespace MusicLibrary {

class IMusicTagParser;
class IMusicLibraryDAO;
class MusicTagParserHost;
class IMusicTagParser;
class LIBPHOENIXPLAYER_EXPORT LocalMusicScannerThread : public QThread
{
    Q_OBJECT
    friend class LocalMusicScanner;
protected:
    explicit LocalMusicScannerThread(PPSettings *set, PluginLoader *loader, QObject *parent = Q_NULLPTR);
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

private:
    QStringList             m_pathList;
    bool                    m_stopLookupFlag;
    QMimeDatabase           m_QMimeDatabase;
    QMutex                  m_mutex;

    PPSettings                      *m_settings;
    PluginLoader                    *m_pluginLoader;
    IMusicLibraryDAO                *m_dao;
    QList<MusicTagParserHost *>     m_tagHostList;
    QList<IMusicTagParser *>        m_tagParserList;
};
} //MusicLibrary
} //PhoenixPlayer

#endif // LOCALMUSICSCANNERTHREAD_H
