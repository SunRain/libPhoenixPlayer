#ifndef TAGPARSERMANAGER_H
#define TAGPARSERMANAGER_H

#include <QObject>
#include <QPointer>
#include <QList>

namespace PhoenixPlayer {

class SongMetaData;
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

    void setPluginPath(const QString &path);
    void setPlayListDAO(IPlayListDAO *dao = 0);
    void addItem(SongMetaData *data = 0, bool startImmediately = false);
    bool startParserLoop();

protected:
    void initPlugin();
    void parserNextItem();
    void parserItem(SongMetaData *data);
signals:
    void parserPending();
    void parserQueueFinished();

public slots:

private:
    QList<SongMetaData*> mMetaList;
    QPointer<IPlayListDAO> mPlayListDAO;
    QList<IMusicTagParser *> mPluginList;
    QString mPluginPath;
};

} //MusicLibrary
} //PhoenixPlayer
#endif // TAGPARSERMANAGER_H
