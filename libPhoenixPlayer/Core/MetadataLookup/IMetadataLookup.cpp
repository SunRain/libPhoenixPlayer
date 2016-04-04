#include "MetadataLookup/IMetadataLookup.h"

#include "AudioMetaObject.h"

namespace PhoenixPlayer{

namespace MetadataLookup {

IMetadataLookup::IMetadataLookup(QObject *parent)
    : QObject(parent)
{
    m_type = LookupType::TypeUndefined;
}

IMetadataLookup::~IMetadataLookup()
{

}

void IMetadataLookup::setCurrentLookupFlag(IMetadataLookup::LookupType type) {
    m_type = type;
}

IMetadataLookup::LookupType IMetadataLookup::currentLookupFlag() {
    return m_type;
}

} //MetadataLookup
} //PhoenixPlayer
