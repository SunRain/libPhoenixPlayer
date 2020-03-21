#ifndef TRACKSEARCHPROVIDER_H
#define TRACKSEARCHPROVIDER_H

#include <QObject>
#include <QThreadPool>
#include <QList>

#include "ITrackSearch.h"

namespace PhoenixPlayer {
    class PluginMetaData;

namespace DataProvider {

class TrackSearchProvider : public QObject
{
    Q_OBJECT
public:
    explicit TrackSearchProvider(QObject *parent = Q_NULLPTR);
    virtual ~TrackSearchProvider();

    QList<PluginMetaData> enabledPlugins() const;

    void search(const QString &pattern, ITrackSearch::MatchTypes type);

    void searchBy(const QString &pattern, ITrackSearch::MatchTypes type, const PluginMetaData &plugin);

    /*!
     * \brief trim
     * key is uri in MatchObject, value is a list that have the same uri of MatchObject
     * \param objList
     * \return
     */
    static QMap<QString, QList<MatchObject>> trim(const QList<MatchObject> &objList);

Q_SIGNALS:
    void matched(const QString &pattern, const QList<MatchObject> &objList);

private:
    QThreadPool             *m_threadPool = Q_NULLPTR;
    QList<PluginMetaData>   m_pluginMetaList;
};


} // namespace DataProvider
} // namespace PhoenixPlayer



#endif // TRACKSEARCHPROVIDER_H
