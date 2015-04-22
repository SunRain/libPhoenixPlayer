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
class PluginLoader;
namespace MusicLibrary {

class IPlayListDAO;
class DiskLookup;
//class PlayListDAOLoader;
class TagParserManager;
class MusicLibraryManager : public QObject
{
    Q_OBJECT
public:
    explicit MusicLibraryManager(QObject *parent = 0);
    virtual ~MusicLibraryManager();

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    static MusicLibraryManager *instance();
#endif
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
    /// \brief playingSong 返回当前播放的歌曲hash,
    /// 如果当前未播放,则返回数据库中的第一首歌曲,或者播放器上次播放的歌曲
    /// \return
    ///
    Q_INVOKABLE QString playingSongHash();

    ///
    /// \brief setPlayingSongHash 设置需要播放的歌曲在曲库中的hash值
    /// 当设置的hash值在曲库中时会发送 playingSongChanged 信号
    /// \param newHash
    ///
    Q_INVOKABLE void setPlayingSongHash(const QString &newHash);

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
    /// \brief randomSong 转到随机的一首歌曲
    ///
    Q_INVOKABLE void randomSong();

    ///
    /// \brief querySongTitle 按照SongTitle=>FileName的顺序返回歌曲信息
    /// \param hash
    /// \return 如果不存在，则返回空
    ///
    Q_INVOKABLE QString querySongTitle(const QString &hash);

    ///
    /// \brief querySongImage 歌曲封面图片，
    /// 按照
    /// CoverArtMiddle
    /// CoverArtLarge
    /// CoverArtSmall
    /// AlbumImageUrl
    /// ArtistImageUri
    /// 顺序返回图片地址，如果都不存在，则返回空
    /// \param hash
    /// \return
    ///
    Q_INVOKABLE QString querySongImageUri(const QString &hash);

    QStringList queryMusicLibrary(Common::SongMetaTags targetColumn,
                                    Common::SongMetaTags regColumn,
                                    const QString &regValue, bool skipDuplicates = true);
    ///
    /// \brief querySongMetaElement 查询某一个列
    /// \param targetColumn 需要查询的列
    /// \param hash hash值
    /// \param skipDuplicates 是否跳过重复值
    /// \return
    ///
    QStringList querySongMetaElement(Common::SongMetaTags targetColumn,
                                     const QString &hash,
                                     bool skipDuplicates = true);

    ///
    /// \brief querySongMetaElement 由于qml不支持直接和cpp代码传递enum值,所以用int代替
    /// \param columnIndex
    /// \param hash
    /// \param skipDuplicates
    /// \return
    ///
    Q_INVOKABLE QStringList querySongMetaElementByIndex(int columnIndex,
                                const QString &hash, bool skipDuplicates = true);

    ///
    /// \brief queryPlayListElement 搜索播放列表相关内容, 如果hash值为空
    /// \param targetColumn 需要搜索的列
    /// \param hash 条件hash值
    /// \return
    ///
    QStringList queryPlayListElement(Common::PlayListElement targetColumn,
                                     const QString &hash);

    ///
    /// \brief queryPlayListElement 由于qml不支持直接和cpp代码传递enum值,所以用int代替
    /// \param targetColumn 需要搜索的列
    /// \param hash 条件hash值
    /// \return
    ///
    Q_INVOKABLE QStringList queryPlayListElementByIndex(int index,
                                                        const QString &hash );

    Q_INVOKABLE bool insertToPlayList(const QString &playListHash,
                                      const QString &newSongHash);

    Q_INVOKABLE bool deleteFromPlayList(const QString &playListHash,
                                        const QString &songHash,
                                        bool deleteFromStorage = false);

    QString queryOne(const QString &hash, Common::SongMetaTags tag, bool skipDuplicates = true);
    Q_INVOKABLE QString queryOneByIndex(const QString &hash, int tag, bool skipDuplicates = true);
//protected:
//    bool init();

signals:
    void searching (QString path, QString file, qint64 size);
    void searchingFinished();
    void playingSongChanged();
    void playListChanged();
public slots:
//    void fileFound (QString path, QString file, qint64 size);

private:
      bool init();
      void initTagParserManager();
//    void initSettings(/*Settings *settings = 0*/);
//    void initPluginLoader(/*PluginLoader *loader = 0*/);
private:
      bool isInit;
      QPointer<IPlayListDAO> mPlayListDAO;
      PluginLoader *mPluginLoader;
      Settings *mSettings;

      QThread *mDiskLooKupThread;
      QThread *mTagParserThread;
      DiskLookup *mDiskLooKup;
      TagParserManager *mTagParserManager;

      QString mCurrentSongHash;
      QString mCurrentPlayListHash;
};

} //MusicLibrary
} //PhoenixPlayer

#endif // MUSICLIBRARYMANAGER_H
