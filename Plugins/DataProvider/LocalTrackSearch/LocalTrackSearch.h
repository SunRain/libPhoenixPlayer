#ifndef LOCALTRACKSEARCH_H
#define LOCALTRACKSEARCH_H

#include "DataProvider/ITrackSearch.h"

namespace PhoenixPlayer {

    namespace MusicLibrary {
        class MusicLibraryManager;
    }
namespace DataProvider {
namespace LocalTrackSearch {

class LocalTrackSearch : public ITrackSearch
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.ITrackSearch.LocalTrackSearch" FILE "LocalTrackSearch.json")
    Q_INTERFACES(PhoenixPlayer::BasePlugin)
public:
    explicit LocalTrackSearch(QObject *parent = Q_NULLPTR);
    virtual ~LocalTrackSearch() override;

    // BasePlugin interface
public:
    virtual PluginProperty property() const Q_DECL_OVERRIDE;
    virtual PluginType type() const Q_DECL_OVERRIDE;

    // IDataProvider interface
public:
    virtual bool support(SupportedTypes type) const Q_DECL_OVERRIDE;

    // ITrackSearch interface
public:
    virtual QList<MatchObject> match(const QString &pattern, MatchTypes type) Q_DECL_OVERRIDE;

private:
    MusicLibrary::MusicLibraryManager *m_libMgr = Q_NULLPTR;
};



} // namespace LocalTrackSearch
} // namespace DataProvider
} // namespace PhoenixPlayer

#endif // LOCALTRACKSEARCH_H
