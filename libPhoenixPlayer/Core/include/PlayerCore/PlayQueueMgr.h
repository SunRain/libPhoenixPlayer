#ifndef PLAYQUEUEMGR_H
#define PLAYQUEUEMGR_H

#include <QObject>
#include "PlayListMgr.h"
namespace PhoenixPlayer {
class PlayQueueMgr : public PlayListMgr
{
    Q_OBJECT
public:
    explicit PlayQueueMgr(QObject *parent = 0);

    // PlayListMgr interface
public:
    QStringList existPlayLists() const;
    bool open(const QString &name);
    bool save(const QString &fileName);

protected:
    void queryPlayLists();


};
} //PhoenixPlayer
#endif // PLAYQUEUEMGR_H
