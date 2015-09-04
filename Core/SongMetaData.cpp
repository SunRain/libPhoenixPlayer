#include <QDebug>

#include "SongMetaData.h"

namespace PhoenixPlayer {

SongMetaData::SongMetaData(QObject *parent) : QObject(parent)
{
    for (int i = (int)Common::SongMetaTags::E_FirstFlag +1;
         i < (int)Common::SongMetaTags::E_LastFlag; ++i) {
        m_metaHash.insert (Common::SongMetaTags(i), QVariant());
    }
}

SongMetaData::~SongMetaData()
{
    if (!m_metaHash.isEmpty ())
        m_metaHash.clear ();
}

void SongMetaData::setMeta(Common::SongMetaTags tagType, const QVariant &value)
{
    if (tagType == Common::SongMetaTags::E_FirstFlag
            || tagType == Common::SongMetaTags::E_LastFlag) {
        qDebug()<<"Invalid SongMetaTag Type "<<tagType;
    }
    m_metaHash[tagType] = value;
}

QVariant SongMetaData::getMeta(Common::SongMetaTags tagType) const
{
    if (tagType == Common::SongMetaTags::E_FirstFlag
            || tagType == Common::SongMetaTags::E_LastFlag) {
        qDebug()<<"Invalid SongMetaTag Type "<<tagType;
    }
    return m_metaHash[tagType];
}

QString SongMetaData::toString()
{
    Common c;
    QString str;
    for (int i = (int)Common::SongMetaTags::E_FirstFlag +1;
         i < (int)Common::SongMetaTags::E_LastFlag - 1; ++i) {
        str += QString("%1 = [%2], ")
                .arg(c.enumToStr("SongMetaTags", i))
                .arg(getMeta(Common::SongMetaTags(i)).toString());
    }


    str += QString("%1 = [%2]")
            .arg(c.enumToStr("SongMetaTags",(int)Common::SongMetaTags::E_LastFlag -1))
            .arg(getMeta(Common::SongMetaTags((int)Common::SongMetaTags::E_LastFlag -1)).toString());
    return str;
}

bool SongMetaData::operator ==(const SongMetaData &other) const
{
    return m_metaHash[Common::E_Hash].toString () == other.getMeta (Common::E_Hash).toString ();
}

}//PhoenixPlayer
