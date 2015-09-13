#ifndef TAGPARSERMANAGER_H
#define TAGPARSERMANAGER_H

#include <QObject>
#include <QPointer>
#include <QList>
#include <QStringList>
#include <QMutex>

namespace PhoenixPlayer {

class SongMetaData;
class PluginLoader;
namespace MusicLibrary {

class MusicLibrary;
class IPlayListDAO;
class IMusicTagParser;
class TagParserManager : public QObject
{
    Q_OBJECT
public:
    explicit TagParserManager(QObject *parent = 0);
    virtual ~TagParserManager();

    void addItem(SongMetaData *data = 0, bool startImmediately = false);
//    void parserImmediately(const QList<SongMetaData *> &list);
    void parserImmediately(QList<SongMetaData *> *list);

    bool startParserLoop();

protected:
    void parserNextItem();
    void parserItem(SongMetaData *data);
signals:
    void parserPending();
    void parserQueueFinished();

private:
    void setPluginLoader();
    void setPlayListDAO();
private:
    QList<SongMetaData*> m_metaList;
    QPointer<IPlayListDAO> m_playListDAO;
    PluginLoader *m_pluginLoader;
    QList<IMusicTagParser *> m_pluginList;
    QString m_pluginPath;
    QStringList m_pluginHashList;
    int m_currentIndex;
    QMutex m_mutex;
};

} //MusicLibrary
} //PhoenixPlayer
#endif // TAGPARSERMANAGER_H
