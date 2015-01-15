#ifndef MUSICLIBRARYMANAGER_H
#define MUSICLIBRARYMANAGER_H

#include <QObject>
#include "Settings.h"

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


protected:
    bool init();

signals:

public slots:
    void fileFound(QString path, QString file, qint64 size);

private:
    IPlayListDAO *mPlayListDAO;
    DiskLookup *mDiskLooKup;
    PlayListDAOLoader *mDAOLoader;

    QThread *mThread;

    Settings *mSettings;
};

} //PlayList
} //PhoenixPlayer

#endif // MUSICLIBRARYMANAGER_H
