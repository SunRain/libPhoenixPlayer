#ifndef MUSICLIBRARYMANAGER_H
#define MUSICLIBRARYMANAGER_H

#include <QObject>
#include <QPointer>
#include <QStringList>

//#include "Settings.h"
#include "Common.h"

class QThread;

namespace PhoenixPlayer {
class Settings;
namespace MusicLibrary {

class IPlayListDAO;
class DiskLookup;
class PlayListDAOLoader;
class TagParserManager;
class MusicLibraryManager : public QObject
{
    Q_OBJECT
public:
    explicit MusicLibraryManager(QObject *parent = 0);
    virtual ~MusicLibraryManager();
    static MusicLibraryManager *getInstance();

    void setSettings(Settings *settings = 0);

    ///
    /// \brief scanLocalMusic 搜索本地音乐文件
    /// \return
    ///
    Q_INVOKABLE bool scanLocalMusic();

    ///
    /// \brief changePlayList 更改播放列表
    /// \param playListHash
    /// \return
    ///
    Q_INVOKABLE bool changePlayList(const QString &playListHash);

    Q_INVOKABLE bool createPlayList(const QString &playListName);

    Q_INVOKABLE bool deletePlayList(const QString &playListHash);

    ///
    /// \brief playingSong 返回当前播放的歌曲hash,如果当前未播放,则返回数据库中的第一首歌曲,或者播放器上次播放的歌曲
    /// \return
    ///
    Q_INVOKABLE QString playingSongHash();
    QString firstSongHash();
    QString lastSongHash();

    ///
    /// \brief nextSong 当前播放列表的下一首歌曲,如果在末尾则返回第一首歌
    ///
    Q_INVOKABLE void nextSong();
    ///
    /// \brief preSong 当前播放列表的上一首歌曲,如果在第一首,则返回最后一首
    ///
    Q_INVOKABLE void preSong();
    ///
    /// \brief randomSong 随机歌曲hash
    ///
    Q_INVOKABLE void randomSong();

    ///
    /// \brief querySongMetaElement 查询某一个列
    /// \param targetColumn 需要查询的列
    /// \param hash hash值
    /// \param skipDuplicates 是否跳过重复值
    /// \return
    ///
    QStringList querySongMetaElement(Common::SongMetaTags targetColumn = Common::E_FirstFlag,
                                const QString &hash = "", bool skipDuplicates = true);
    ///
    /// \brief querySongMetaElement 由于qml不支持直接和cpp代码传递enum值,所以用int代替
    /// \param columnIndex
    /// \param hash
    /// \param skipDuplicates
    /// \return
    ///
    Q_INVOKABLE QStringList querySongMetaElementByIndex(int columnIndex = 0,
                                const QString &hash = "", bool skipDuplicates = true);

    ///
    /// \brief queryPlayListElement 搜索播放列表相关内容, 如果hash值为空
    /// \param targetColumn 需要搜索的列
    /// \param hash 条件hash值
    /// \return
    ///
    QStringList queryPlayListElement(Common::PlayListElement targetColumn = Common::E_PlayListNullElement, const QString &hash = "");

    ///
    /// \brief queryPlayListElement 由于qml不支持直接和cpp代码传递enum值,所以用int代替
    /// \param targetColumn 需要搜索的列
    /// \param hash 条件hash值
    /// \return
    ///
    Q_INVOKABLE QStringList queryPlayListElementByIndex(int index = 0, const QString &hash = "");

    Q_INVOKABLE bool insertToPlayList(const QString &playListHash, const QString &newSongHash);
    Q_INVOKABLE bool deleteFromPlayList(const QString &playListHash, const QString &songHash, bool deleteFromStorage = false);

protected:
    bool init();

signals:
    void searching (QString path, QString file, qint64 size);
    void searchingFinished();
    void playingSongChanged();
    void playListChanged();
public slots:
//    void fileFound (QString path, QString file, qint64 size);

private:
    QPointer<IPlayListDAO> mPlayListDAO;
    PlayListDAOLoader *mDAOLoader;

    QPointer<QThread> mDiskLooKupThread;
    QPointer<QThread> mTagParserThread;
    QPointer<DiskLookup> mDiskLooKup;
    QPointer<Settings> mSettings;
    QPointer<TagParserManager> mTagParserManager;
    QString mCurrentSongHash;
    QString mCurrentPlayListHash;
};

} //MusicLibrary
} //PhoenixPlayer

#endif // MUSICLIBRARYMANAGER_H
