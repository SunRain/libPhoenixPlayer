#ifndef LOCALMUSICSCANNER_H
#define LOCALMUSICSCANNER_H

#include <QObject>

namespace PhoenixPlayer {

class Settings;
class PluginLoader;
class SongMetaData;
namespace MusicLibrary {

class IMusicLibraryDAO;
class LocalMusicScannerThread;
class LocalMusicScanner : public QObject
{
    Q_OBJECT
public:
    explicit LocalMusicScanner(QObject *parent = 0);
    virtual ~LocalMusicScanner();
    Q_INVOKABLE void scanLocalMusic();

signals:
//    void searching (const QString &path, const QString &file, const qint64 &size);
    void searchingFinished();

public slots:

private slots:
//    void appendToList(const QString &path, const QString &file, const qint64 &size);
//    void parserTag(const QList<SongMetaData *> &list);

private:
//    AsyncDiskLookup *m_asyncDiskLookup;
//    AsyncTagParserMgrWrapper *m_tagParserWrapper;
//    QList<PhoenixPlayer::SongMetaData *> m_metaDataList;
    Settings *m_settings;
//    PluginLoader *m_pluginLoader;
//    IMusicLibraryDAO *m_dao;
    LocalMusicScannerThread *m_scanner;

};
} //MusicLibrary
} //PhoenixPlayer

#endif // LOCALMUSICSCANNER_H
