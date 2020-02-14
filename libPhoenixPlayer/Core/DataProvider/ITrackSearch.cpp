#include "DataProvider/ITrackSearch.h"

namespace PhoenixPlayer {
namespace DataProvider {

//class MatchObjectPriv : public QSharedData
//{
//public:
//    MatchObjectPriv() {}
//    ~MatchObjectPriv() {}

//    MatchObject::ObjectType objectType = MatchObject::TypeUndefined;
//    ITrackSearch::MatchType matchType = ITrackSearch::MatchType::MatchUndefined;
//    QString uri = QString();
//    QString matchedStr = QString();
//    PluginProperty pluginProperty;
//};

ITrackSearch::ITrackSearch(QObject *parent)
    : IDataProvider(parent)
{

}

ITrackSearch::~ITrackSearch()
{

}

MatchObject::MatchObject()
    : d(new MatchObjectPriv())
{

}

MatchObject::~MatchObject()
{

}

bool MatchObject::operator ==(const MatchObject &other)
{
    return other.d.data()->uri == d.data()->uri &&
            other.d.data()->matchIndex == d.data()->matchIndex &&
            other.d.data()->matchedLength == d.data()->matchedLength &&
            other.d.data()->matchType == d.data()->matchType &&
            other.d.data()->matchedStr == d.data()->matchedStr &&
            other.d.data()->queryStr == d.data()->queryStr &&
            other.d.data()->objectType == d.data()->objectType &&
            other.d.data()->pluginProperty.name == d.data()->pluginProperty.name;
}

PluginProperty MatchObject::pluginProperty() const
{
    return d.data()->pluginProperty;
}

void MatchObject::setPluginProperty(const PluginProperty &property)
{
    d.data()->pluginProperty = property;
}

QString MatchObject::uri() const
{
    return d.data()->uri;
}

void MatchObject::setUri(const QString &uri)
{
    d.data()->uri = uri;
}

QString MatchObject::matchedStr() const
{
    return d.data()->matchedStr;
}

void MatchObject::setMatchedStr(const QString &str)
{
    d.data()->matchedStr = str;
}

int MatchObject::matchedIndex() const
{
    return d.data()->matchIndex;
}

void MatchObject::setMatchedIndex(int idx)
{
    d.data()->matchIndex = idx;
}

int MatchObject::matchedLength() const
{
    return d.data()->matchedLength;
}

void MatchObject::setMatchedLength(int length)
{
    d.data()->matchedLength = length;
}

QString MatchObject::queryStr() const
{
    return d.data()->queryStr;
}

void MatchObject::setQueryStr(const QString &str)
{
    d.data()->queryStr = str;
}

MatchObject::ObjectType MatchObject::objectType() const
{
    return d.data()->objectType;
}

void MatchObject::setObjectType(MatchObject::ObjectType type)
{
    d.data()->objectType = type;
}

ITrackSearch::MatchType MatchObject::matchType() const
{
    return d.data()->matchType;
}

void MatchObject::setMatchType(ITrackSearch::MatchType type)
{
    d.data()->matchType = type;
}

MatchObject::MatchObjectPriv::MatchObjectPriv()
{

}

MatchObject::MatchObjectPriv::~MatchObjectPriv()
{

}



} // namespace DataProvider
} // namespace PhoenixPlayer
