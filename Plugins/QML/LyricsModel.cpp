
#include <QDebug>

#include "LyricsModel.h"
#include "LyricsParser.h"

namespace PhoenixPlayer {
namespace QmlPlugin {


LyricsModel::LyricsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    mCurrentIndex = 0;
}

LyricsModel::~LyricsModel()
{
    if (!mLyricsList.isEmpty ())
        mLyricsList.clear ();
}

void LyricsModel::setLyricsStr(const QString &lyrics)
{
    if (lyrics.isEmpty ()) {
        qDebug()<<"LyricsModel lyrics is empty";
        MetadataLookup::QLyrics qc;
        qc.time = QTime();
        qc.lyrics = "No lyrics";
        mLyricsList.append (qc);
        return;
    }

    mLyricsList = MetadataLookup::LyricParser::parserLyrics (lyrics);
    if (mLyricsList.isEmpty ()) {
        MetadataLookup::QLyrics qc;
        qc.time = QTime();
        qc.lyrics = "No lyrics";
        mLyricsList.append (qc);
        return;
    }
    for (int i=0; i<mLyricsList.size (); ++i) {
        beginInsertRows (QModelIndex(), i, i);
        endInsertRows ();
    }
}

void LyricsModel::findIndex(quint64 tick)
{
    if (tick == 0) {
        setCurrentIndex (0);
        return;
    }
    /* 100s
     * 100/60 = 1 min
     * 100 %60 = 40s
     *
     */

    QTime time(0, tick/60, tick % 60, 0);

    qDebug()<<"LyricsModel findIndex for tick "<<tick<<" time "<< time.toString ();

    int index = currentIndex ();
    if (time < mLyricsList.at (index).time) { //jump before
        for (int i = index -1;
             i >= 0; --i) {
            if (mLyricsList.at (i).time < time
                    && mLyricsList.at (i+1).time > time) {
                index = i;
                break;
            }
        }
    } else if (index == mLyricsList.size () -2
               && time >= mLyricsList.at (mLyricsList.size () -1).time) {
        index++;
    } else if (index < mLyricsList.size () -2) {
        for (int i = index;
             i < mLyricsList.size (); ++i) {
            if (mLyricsList.at (i).time > time) {
                index = i > 0 ? i-1 : 0;
                break;
            }
        }
    } else {
        index = 0;
    }
    setCurrentIndex (index);
}

int LyricsModel::currentIndex()
{
    return mCurrentIndex;
}

void LyricsModel::setCurrentIndex(int index)
{
    if ((index == 0 || index < mLyricsList.size ())
            && mCurrentIndex != index) {
        mCurrentIndex = index;
        qDebug()<<"LyricsModel setCurrentIndex mCurrentIndex "<< mCurrentIndex;
        emit currentIndexChanged ();
    }
}

int LyricsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mLyricsList.size ();
}

QVariant LyricsModel::data(const QModelIndex &index, int role) const
{
    if (index.row () < 0 || index.row () >= mLyricsList.size ())
        return QVariant();
    MetadataLookup::QLyrics lyrics = mLyricsList.at (index.row ());
    switch (role) {
    case LyricsRoles::TimeRole:
        return lyrics.time;
    case LyricsRoles::TextRole:
        return lyrics.lyrics;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> LyricsModel::roleNames() const
{
    QHash<int, QByteArray> role;
    role.insert (LyricsRoles::TimeRole, "time");
    role.insert (LyricsRoles::TextRole, "lyricsText");
    return role;
}


} //QmlPlugin
} //PhoenixPlayer
