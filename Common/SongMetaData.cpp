#include <QDebug>

#include "SongMetaData.h"

namespace PhoenixPlayer {

SongMetaData::SongMetaData(QObject *parent) : QObject(parent)
{
    for (int i = (int)Common::SongMetaTags::E_FirstFlag +1;
         i < (int)Common::SongMetaTags::E_LastFlag; ++i) {
        mMetaHash.insert (Common::SongMetaTags(i), QVariant());
    }
}

SongMetaData::~SongMetaData()
{
    if (!mMetaHash.isEmpty ())
        mMetaHash.clear ();
}

void SongMetaData::setMeta(Common::SongMetaTags tagType, const QVariant &value)
{
    if (tagType == Common::SongMetaTags::E_FirstFlag
            || tagType == Common::SongMetaTags::E_LastFlag) {
        qDebug()<<"Invalid SongMetaTag Type "<<tagType;
    }
    mMetaHash[tagType] = value;
}

QVariant SongMetaData::getMeta(Common::SongMetaTags tagType)
{
    if (tagType == Common::SongMetaTags::E_FirstFlag
            || tagType == Common::SongMetaTags::E_LastFlag) {
        qDebug()<<"Invalid SongMetaTag Type "<<tagType;
    }
    return mMetaHash[tagType];
}

}//PhoenixPlayer
