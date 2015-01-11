//#include <QStringList>

//#include "IPlayList.h"

//IPlayList::IPlayList(QObject *parent) :
//    QObject(parent)
//{
//}


//PhoenixPlayerCore::BasePlayList *PhoenixPlayerCore::BasePlayList::getInstance()
//{
//    static BasePlayList list;
//    return &list;
//}

//void PhoenixPlayerCore::BasePlayList::addSong(PhoenixPlayerCore::SongMetaDate *data)
//{
//    mSongList.append(data);
//}

//void PhoenixPlayerCore::BasePlayList::addSong(QList<PhoenixPlayerCore::SongMetaDate *> &list)
//{
//    foreach (SongMetaDate *date, list) {
//        mSongList.append(data);
//    }
//}

//PhoenixPlayerCore::BasePlayList::~BasePlayList()
//{
//    mSongList.clear();
//}

//void PhoenixPlayerCore::BasePlayList::addSong(const QString &fullPathSong)
//{
//    mSongList.append(fullPathSong);
//}

//void PhoenixPlayerCore::BasePlayList::deleteSong(const QString &fullPathSong)
//{
//    int index = -1;
//    for (int i=0; i<mSongList.size(); ++i) {
//        QString file = mSongList.at(i);
//        if (file.toLower() == fullPathSong.toLower()) {
//            index = i;
//            break;
//        }
//    }
//    if (index >=0) {
//        mSongList.removeAt(index);
//    }
//}

//void PhoenixPlayerCore::BasePlayList::deleteSong(const QString &fullPathSong)
//{
//    int index = -1;
//    for (int i=0; i<mSongList.size(); ++i) {
//        SongMetaDate *data = mSongList.at(i);
//        QString tmp = QString("%1/%2").arg(data->filePath).arg(data->fileName);
//        if (tmp.toLower() == fullPathSong.toLower()) {
//            index = i;
//            break;
//        }
//    }
//    if (index >=0) {
//        mSongList.removeAt(index);
//    }
//}
