#ifndef MUSICLIBRARYMANAGER_H
#define MUSICLIBRARYMANAGER_H

#include <QObject>
#include "Settings.h"
#include "Common.h"

class QThread;
namespace PhoenixPlayer {
namespace PlayList {

class IPlayListDAO;
class DiskLookup;
class PlayListDAOLoader;
class MusicLibraryManager : public QObject
{
    Q_OBJECT
public:
    explicit MusicLibraryManager(QObject *parent = 0);
    virtual ~MusicLibraryManager();
    static MusicLibraryManager *getInstance();

    void setSettings(Settings *settings = 0);

    Q_INVOKABLE bool scanLocalMusic();

    ///
    /// \brief playingSong 返回当前播放的歌曲hash,如果当前未播放,则返回数据库中的第一首歌曲,或者播放器上次播放的歌曲
    /// \return
    ///
    Q_INVOKABLE QString playingSong();

    Q_INVOKABLE void nextSong();
    Q_INVOKABLE void preSong();
    Q_INVOKABLE void randomSong();

    Q_INVOKABLE QStringList querySongMetaElement(Common::MusicLibraryElement targetColumn = Common::E_NULLElement,
                                const QString &hash = "", bool skipDuplicates = true);

protected:
    bool init();

signals:
    void searching (QString path, QString file, qint64 size);
    void searchingFinished();
    void playingSongChanged();
public slots:
    void fileFound (QString path, QString file, qint64 size);

private:
    IPlayListDAO *mPlayListDAO;
    DiskLookup *mDiskLooKup;
    PlayListDAOLoader *mDAOLoader;

    QThread *mThread;

    Settings *mSettings;

    QString mCurrentSongHash;
};

} //PlayList
} //PhoenixPlayer

#endif // MUSICLIBRARYMANAGER_H
