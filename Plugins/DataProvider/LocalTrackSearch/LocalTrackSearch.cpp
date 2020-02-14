#include "LocalTrackSearch.h"

#include <QRegExp>
#include <QDebug>

#include "AudioMetaObject.h"
#include "MusicLibrary/MusicLibraryManager.h"

namespace PhoenixPlayer {
namespace DataProvider {
namespace LocalTrackSearch {

LocalTrackSearch::LocalTrackSearch(QObject *parent)
    : ITrackSearch(parent)
{
//    m_libMgr = new MusicLibrary::MusicLibraryManager(this);
}

LocalTrackSearch::~LocalTrackSearch()
{
    if (m_libMgr) {
        m_libMgr->deleteLater();
        m_libMgr = Q_NULLPTR;
    }
}

PluginProperty LocalTrackSearch::property() const
{
    return PluginProperty("LocalTrackSearch",
                          "1.0",
                          "Local music search",
                          false,
                          false);
}

BasePlugin::PluginType LocalTrackSearch::type() const
{
    return BasePlugin::PluginDataProvider;
}

bool LocalTrackSearch::support(IDataProvider::SupportedTypes type) const
{
    if ((type & SupportTrackSearch) == SupportTrackSearch) {
        return true;
    }
    return false;
}

QList<MatchObject> LocalTrackSearch::match(const QString &pattern, ITrackSearch::MatchTypes type)
{
    if (!m_libMgr) {
         m_libMgr = new MusicLibrary::MusicLibraryManager(this);
    }

    QList<MatchObject> objList;

#define DO_REG_EXP(reg_pattern, str, path, matchType) \
    QRegExp rx(reg_pattern); \
    int pos = 0; \
    while ((pos = rx.indexIn(str, pos)) != -1) { \
        pos += rx.matchedLength(); \
        MatchObject obj; \
        obj.setPluginProperty(this->property()); \
        obj.setUri(path); \
        obj.setQueryStr(reg_pattern); \
        obj.setMatchType(matchType); \
        obj.setObjectType(MatchObject::TypeLocalFile); \
        obj.setMatchedIndex(pos); \
        obj.setMatchedLength(rx.matchedLength()); \
        obj.setMatchedStr(str); \
        objList.append(obj); \
    }

    AudioMetaList list = m_libMgr->allTracks();
    foreach (const auto &it, list) {
        if ((type & ITrackSearch::MatchFilePath) == ITrackSearch::MatchFilePath) {
            const QString path = it.uri().toLocalFile();
//            QRegExp rx(str);
//            int pos = 0;
//            while ((pos = rx.indexIn(path, pos)) != -1) {
//                pos += rx.matchedLength();
//                MatchObject obj;
//                obj.setPluginProperty(this->property());
//                obj.setUri(path);
//                obj.setQueryStr(str);
//                obj.setMatchType(ITrackSearch::MatchFilePath);
//                obj.setObjectType(MatchObject::TypeLocalFile);
//                obj.setMatchedIndex(pos);
//                obj.setMatchedLength(rx.matchedLength());
//                obj.setMatchedStr(path);
//                objList.append(obj);
//            }
            DO_REG_EXP(pattern, path, path, ITrackSearch::MatchFilePath)
        }
        if ((type & ITrackSearch::MatchAlbumName) == ITrackSearch::MatchAlbumName) {
            const QString path = it.uri().toLocalFile();
            const QString str = it.albumMeta().name();
            DO_REG_EXP(pattern, str, path, ITrackSearch::MatchAlbumName)
        }
        if ((type & ITrackSearch::MatchTrackName) == ITrackSearch::MatchTrackName) {
            const QString path = it.uri().toLocalFile();
            const QString str = it.trackMeta().title();
            DO_REG_EXP(pattern, str, path, ITrackSearch::MatchTrackName)
        }
        if ((type & ITrackSearch::MatchArtistName) == ITrackSearch::MatchArtistName) {
            const QString path = it.uri().toLocalFile();
            const QString str = it.artistMeta().name();
            DO_REG_EXP(pattern, str, path, ITrackSearch::MatchArtistName)
        }
    }
    return objList;
}



} // namespace LocalTrackSearch
} // namespace DataProvider
} // namespace PhoenixPlayer
