#ifndef LOCALMUSICSACNNER_H
#define LOCALMUSICSACNNER_H

#include <QObject>

namespace PhoenixPlayer {

class Settings;
class PluginLoader;
class SongMetaData;
namespace MusicLibrary {

class IPlayListDAO;
class DiskLookup;
//class PlayListDAOLoader;
class TagParserManager;
class AsyncDiskLookup;
class AsyncTagParserMgrWrapper;
class LocalMusicSacnner : public QObject
{
    Q_OBJECT
public:
    explicit LocalMusicSacnner(QObject *parent = 0);
    virtual ~LocalMusicSacnner();
    Q_INVOKABLE void scanLocalMusic();

signals:
//    void searching (const QString &path, const QString &file, const qint64 &size);
    void searchingFinished();

public slots:

private slots:
//    void appendToList(const QString &path, const QString &file, const qint64 &size);
//    void parserTag(const QList<SongMetaData *> &list);

private:
    AsyncDiskLookup *mAsyncDiskLookup;
    AsyncTagParserMgrWrapper *mTagParserWrapper;
    QList<PhoenixPlayer::SongMetaData *> mMetaDataList;
    Settings *mSettings;
    PluginLoader *mPluginLoader;
    IPlayListDAO *mDao;

};
} //MusicLibrary
} //PhoenixPlayer

#endif // LOCALMUSICSACNNER_H
