#ifndef RECENTPLAYEDMGR_H
#define RECENTPLAYEDMGR_H

#include <QObject>
#include <QList>
#include <QStringList>

#include "libphoenixplayer_global.h"
#include "PlayerCore/MusicQueue.h"

namespace PhoenixPlayer {
//class AudioMetaObject;
//class PluginLoader;
//class PluginHost;
//class Settings;

//namespace MusicLibrary {
//class IMusicLibraryDAO;
//}

class RecentPlayedMgr : public MusicQueue
{
    Q_OBJECT
public:
    explicit RecentPlayedMgr(QObject *parent = 0);
    virtual ~RecentPlayedMgr();

private:
    void queryList();
    void save();
private:
    QString m_file;

};

} //PhoenixPlayer
#endif // RECENTPLAYEDMGR_H
