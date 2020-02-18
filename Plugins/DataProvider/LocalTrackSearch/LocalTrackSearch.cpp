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

#define DO_REG_EXP(reg_pattern, str, path, matchType, list) \
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
        list.append(obj); \
    }

    AudioMetaList list = m_libMgr->allTracks();
    foreach (const auto &it, list) {
        QList<MatchObject> tmpList;

        if ((type & ITrackSearch::MatchTrackName) == ITrackSearch::MatchTrackName) {
            const QString path = it.uri().toLocalFile();
            const QString str = it.trackMeta().title();
            DO_REG_EXP(pattern, str, path, ITrackSearch::MatchTrackName, tmpList)
        }

        if ((type & ITrackSearch::MatchFilePath) == ITrackSearch::MatchFilePath) {
            const QString path = it.uri().toLocalFile();
            // skip it file path contains track name (file name is same as track name)
            bool skip = false;
            foreach(const auto &node, tmpList) {
                if (node.matchType() == ITrackSearch::MatchTrackName) {
                    const QString tn = it.trackMeta().title();
                    if (path.contains(tn)) {
                        skip = true;
                        break;
                    }
                }
            }
            if (!skip) {
                DO_REG_EXP(pattern, path, path, ITrackSearch::MatchFilePath, tmpList)
            }
        }
        if ((type & ITrackSearch::MatchAlbumName) == ITrackSearch::MatchAlbumName) {
            const QString path = it.uri().toLocalFile();
            const QString str = it.albumMeta().name();
            DO_REG_EXP(pattern, str, path, ITrackSearch::MatchAlbumName, tmpList)
        }
        if ((type & ITrackSearch::MatchArtistName) == ITrackSearch::MatchArtistName) {
            const QString path = it.uri().toLocalFile();
            const QString str = it.artistMeta().name();
            DO_REG_EXP(pattern, str, path, ITrackSearch::MatchArtistName, tmpList)
        }
        objList.append(tmpList);
    }
    return objList;
}



} // namespace LocalTrackSearch
} // namespace DataProvider
} // namespace PhoenixPlayer
