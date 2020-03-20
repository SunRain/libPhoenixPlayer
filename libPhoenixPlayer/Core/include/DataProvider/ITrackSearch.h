#ifndef ITRACKSEARCH_H
#define ITRACKSEARCH_H

#include <QtGlobal>
#include <QObject>
#include <QStringList>
#include <QSharedDataPointer>

#include "libphoenixplayer_global.h"
#include "AudioMetaObject.h"
#include "DataProvider/IDataProvider.h"

namespace PhoenixPlayer {

    namespace DataProvider {

class MatchObjectPriv;
class MatchObject;
class LIBPHOENIXPLAYER_EXPORT ITrackSearch : public IDataProvider
{
    Q_OBJECT
public:
    enum MatchType
    {
        MatchUndefined = 0x0,
        MatchFilePath = 0x1,
        MatchTrackName = 0x2,
        MatchArtistName = 0x4,
        MatchAlbumName = 0x8,
        MatchAll = MatchFilePath | MatchTrackName | MatchArtistName | MatchAlbumName
    };
    Q_DECLARE_FLAGS(MatchTypes, MatchType)
    Q_FLAG(MatchTypes)

    explicit ITrackSearch(QObject *parent = Q_NULLPTR);
    virtual ~ITrackSearch() override;

    virtual QList<MatchObject> match(const QString &pattern, MatchTypes type) = 0;

    // BasePlugin interface
public:
    virtual PluginProperty property() const Q_DECL_OVERRIDE
    {
        return PluginProperty();
    }
    virtual PluginType type() const Q_DECL_OVERRIDE
    {
        return BasePlugin::PluginDataProvider;
    }

    // IDataProvider interface
public:
    virtual bool support(SupportedTypes type) const Q_DECL_OVERRIDE
    {
        return (type & SupportedType::SupportTrackSearch) == SupportedType::SupportTrackSearch;
    }
};

class LIBPHOENIXPLAYER_EXPORT MatchObject
{
//    Q_GADGET
    friend class ITrackSearch;
public:
    enum ObjectType
    {
        TypeUndefined = 0x0,
        TypeLocalFile,
        TypeUrl
    };
//    Q_ENUM(ObjectType)

    explicit MatchObject();
    virtual ~MatchObject();

    inline MatchObject &operator = (const MatchObject &other)
    {
        if (this != &other) {
            d.operator=(other.d);
        }
        return *this;
    }

    /*!
     * \brief operator !=
     * NOTE this will not compare AudioMetaObject
     * \param other
     * \return
     */
    inline bool operator != (const MatchObject &other)
    {
        return !operator == (other);
    }

    /*!
     * \brief operator ==
     * NOTE this will not compare AudioMetaObject
     * \param other
     * \return
     */
    bool operator == (const MatchObject &other);

    PluginProperty pluginProperty() const;

    void setPluginProperty(const PluginProperty &property);

    /*!
     * \brief uri
     * \return file path or url based on ObjectType
     */
    QString uri() const;

    void setUri(const QString &uri);

    /*!
     * \brief matchedStr
     * \return A long str contains query str
     */
    QString matchedStr() const;

    void setMatchedStr(const QString &str);

    /*!
     * \brief matchedIndex
     * \return index of matched query str in matched str
     */
    int matchedIndex() const;

    void setMatchedIndex(int idx);

    /*!
     * \brief matchedLength
     * \return the length of the last matched string, or -1 if there was no match
     */
    int matchedLength() const;

    void setMatchedLength(int length);

    QString queryStr() const;

    void setQueryStr(const QString &str);

    ObjectType objectType() const;

    void setObjectType(ObjectType type);

    ITrackSearch::MatchType matchType() const;

    void setMatchType(ITrackSearch::MatchType type);

    /*!
     * \brief audioMetaObject
     * NOTE: operation == will not compare AudioMetaObject
     * \return
     */
    AudioMetaObject audioMetaObject() const;

    void setAudioMetaObject(const AudioMetaObject &obj);

private:
    class MatchObjectPriv : public QSharedData
    {
    public:
        explicit MatchObjectPriv();
        ~MatchObjectPriv();

        int matchIndex = -1;
        int matchedLength = -1;
        MatchObject::ObjectType objectType = MatchObject::TypeUndefined;
        ITrackSearch::MatchType matchType = ITrackSearch::MatchType::MatchUndefined;
        QString uri = QString();
        QString matchedStr = QString();
        QString queryStr = QString();
        PluginProperty pluginProperty;
        AudioMetaObject audioMetaObj;
    };

private:
    QSharedDataPointer<MatchObjectPriv> d;
};

} // namespace DataProvider
} // namespace PhoenixPlayer

Q_DECLARE_METATYPE(PhoenixPlayer::DataProvider::MatchObject);

#endif // ITRACKSEARCH_H
