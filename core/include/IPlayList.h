#ifndef BASEPLAYLIST_H
#define BASEPLAYLIST_H

#include <QObject>

class QStringList;

namespace PhoenixPlayerCore {

class IPlayList : public QObject
{
    Q_OBJECT
public:
    explicit IPlayList(QObject *parent = 0);
    virtual ~IPlayList();
    virtual QString *getCurrentPlayedSong() = 0;
    virtual QString *getNextSong() = 0;
    virtual QString *getPreviousSong() = 0;
    virtual QString *getShuffleSong() = 0;
    virtual int getSongCount() = 0;
    virtual bool addSong(const QString &fullPath) = 0;
    virtual bool addSong(const QStringList &fullPathList) = 0;
    virtual bool deleteSong(const QString &fullPath, bool deleteOnStorage) = 0;
    virtual bool deleteSong(const QStringList &fullPath, bool deleteOnStorage) = 0;
//signals:
//    void songAdded();
//    void songDeleted();
//    void playedSongChanged();
//public slots:

//private:


//private:
//    QStringList mSongList;
//    QString mCurrentSong;
};
//Q_DECLARE_INTERFACE(BasePlayList, "PhoenixPlayerCore.BasePlayList/1.0")
}
Q_DECLARE_INTERFACE(PhoenixPlayerCore::IPlayList, "PhoenixPlayerCore.BasePlayList/1.0")

#endif // BASEPLAYLIST_H
