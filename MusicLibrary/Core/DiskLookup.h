#ifndef DISKLOOKUP_H
#define DISKLOOKUP_H

#include <QObject>
#include <QStringList>
#include <QMimeDatabase>

class QStringList;
class QMimeDatabase;

namespace PhoenixPlayer {
namespace MusicLibrary {

class DiskLookup : public QObject
{
    Q_OBJECT
public:
    explicit DiskLookup(QObject *parent = 0);
    static DiskLookup *getInstance();
    virtual ~DiskLookup();

    ///
    /// \brief lookup 开始搜索目录，当未setDir的时候使用默认系统音乐目录搜索
    /// \return
    ///
    bool startLookup();

    ///
    /// \brief stopLookup 停止搜索目录
    /// \return
    ///
    void stopLookup();
    void addLookupDir(const QString &dirName, bool lookupImmediately = false);

protected:
    void scanDir(const QString &path);
signals:
    void fileFound(QString path, QString file, qint64 size);
    void pending();
    void finished();

private:
    QStringList mPathList;
//    bool mLookupLock;
    bool mStopLookupFlag;
    QMimeDatabase mQMimeDatabase;
};

} //MusicLibrary
} //PhoenixPlayer
#endif // DISKLOOKUP_H
