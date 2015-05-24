#ifndef TAGPARSERMANAGER_H
#define TAGPARSERMANAGER_H

#include <QObject>
#include <QPointer>
#include <QList>
#include <QStringList>

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
    bool startParserLoop();

protected:
    void parserNextItem();
    void parserItem(SongMetaData *data);
signals:
    void parserPending();
    void parserQueueFinished();

public slots:

private:
    void setPluginLoader();
    void setPlayListDAO();
private:
    QList<SongMetaData*> mMetaList;
    QPointer<IPlayListDAO> mPlayListDAO;
//    QPointer<PluginLoader> mPluginLoader;
    PluginLoader *mPluginLoader;
    QList<IMusicTagParser *> mPluginList;
    QString mPluginPath;
    QStringList mPluginHashList;
    int mCurrentIndex;
};

} //MusicLibrary
} //PhoenixPlayer
#endif // TAGPARSERMANAGER_H
