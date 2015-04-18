#include "MetadataLookup/IMetadataLookup.h"

#include "SongMetaData.h"

namespace PhoenixPlayer{

namespace MetadataLookup {

IMetadataLookup::IMetadataLookup(QObject *parent)
    : QObject(parent)
{
    mType = LookupType::TypeUndefined;
}

IMetadataLookup::~IMetadataLookup()
{

}

void IMetadataLookup::setCurrentLookupFlag(IMetadataLookup::LookupType type) {
    mType = type;
}

IMetadataLookup::LookupType IMetadataLookup::currentLookupFlag() {
    return mType;
}

} //MetadataLookup
} //PhoenixPlayer
