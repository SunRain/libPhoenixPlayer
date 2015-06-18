#ifndef ASYNCDISKLOOKUP_H
#define ASYNCDISKLOOKUP_H

#include <QFileInfo>
#include <QObject>
#include <QList>

class QThread;
namespace PhoenixPlayer {

class PluginLoader;
class SongMetaData;
namespace MusicLibrary {

class DiskLookup;
class AsyncDiskLookup : public QObject
{
    Q_OBJECT
public:
    explicit AsyncDiskLookup(QObject *parent = 0);
    virtual ~AsyncDiskLookup();
    void setLookupDirs(const QStringList &dirList, bool lookupImmediately = false);

signals:
//    void fileFound(const QString &path, const QString &file, const qint64 &size);
    void started();
    void finished(const QList<PhoenixPlayer::SongMetaData *> &list);

public slots:
    void startLookup();
private slots:
    void found(const QString &path, const QString &file, const qint64 &size);
private:
    QThread *mDiskLookupThread;
    DiskLookup *mDiskLookup;
    QList<PhoenixPlayer::SongMetaData *> mMetaDataList;
};

} //MusicLibrary
} //PhoenixPlayer
#endif // ASYNCDISKLOOKUP_H
