#ifndef LOCALMUSICSCANNERINTERNAL_H
#define LOCALMUSICSCANNERINTERNAL_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QMimeDatabase>

#include "PluginMgr.h"

namespace PhoenixPlayer {


class FileListScanner : public QThread
{
    Q_OBJECT
public:
    explicit FileListScanner(QObject *parent = Q_NULLPTR);
    virtual ~FileListScanner() override;

    void addDir(const QString &dir);
    void stop();

    // QThread interface
protected:
    void run() Q_DECL_OVERRIDE;

signals:
    void scanDir(const QString &dir);
    void findFiles(const QStringList &fileList);

private:
    bool            m_stop = false;
    QStringList     m_dirList;
    QMutex          m_locker;
    QMimeDatabase   m_QMimeDatabase;
};


class PluginMgrInternal;
class AudioParser : public QThread
{
    Q_OBJECT
public:
    explicit AudioParser(QSharedPointer<PluginMgrInternal> pluginMgr, QObject *parent = Q_NULLPTR);

    virtual ~AudioParser() override;

    void addFiles(const QStringList &list);

    void stop();

    // QThread interface
protected:
    void run() Q_DECL_OVERRIDE;

signals:
    void parsingFile(const QString &file, int remainingSize);
    void parsed(const AudioMetaObject &obj);
    void parsingFinished();

private:
    bool                                m_stop;
    QSharedPointer<PluginMgrInternal>   m_pluginMgrInner;
    QMutex                              m_mutex;
    QStringList                         m_fileList;
};


class LocalMusicScannerInternal : public QObject
{
    Q_OBJECT
public:
    explicit LocalMusicScannerInternal(QSharedPointer<PluginMgrInternal> pluginMgr,
                                       QObject *parent = Q_NULLPTR);
    virtual ~LocalMusicScannerInternal();

    inline QSharedPointer<FileListScanner> fileListScanner()
    {
        return m_fileListScanner;
    }

    inline QSharedPointer<AudioParser> audioParser()
    {
        return m_audioParser;
    }

private:
    QSharedPointer<FileListScanner>     m_fileListScanner;
    QSharedPointer<AudioParser>         m_audioParser;
    QSharedPointer<PluginMgrInternal>   m_pluginMgrInner;

};

} //PhoenixPlayer
#endif // LOCALMUSICSCANNERINTERNAL_H
