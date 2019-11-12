#ifndef LOCALMUSICSCANNER_H
#define LOCALMUSICSCANNER_H

#include <QObject>
#include <QSharedPointer>

#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {
    class LocalMusicScannerInternal;
    class FileListScanner;
    class AudioParser;

    namespace MusicLibrary {

/*!
 * \brief The LocalMusicScanner class
 * Internal Singleton class
 */
class LIBPHOENIXPLAYER_EXPORT LocalMusicScanner : public QObject
{
    Q_OBJECT
public:
    explicit LocalMusicScanner(QObject *parent = Q_NULLPTR);
    virtual ~LocalMusicScanner();

    void scanLocalMusic();
    void scanDir(const QString &dirname);
    void scarnDirs(const QStringList &list);

signals:
    void searchingDir(const QString &dirName);
    void parsingFile(const QString &file, int remainingSize);
    void newFileListAdded(const QStringList &list);
    void searchingFinished();

private:
    void doScann(const QString &dirname);

    void insert();

private:
    QSharedPointer<LocalMusicScannerInternal>   m_internal;
    QSharedPointer<FileListScanner>             m_fileListScanner;
    QSharedPointer<AudioParser>                 m_audioParser;

    AudioMetaList                               m_audioList;
};
} //MusicLibrary
} //PhoenixPlayer

#endif // LOCALMUSICSCANNER_H
