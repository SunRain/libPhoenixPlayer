#ifndef PLAYLISTDAOLOADER_H
#define PLAYLISTDAOLOADER_H

#include <QObject>

#include "MusicLibrary/IPlayListDAO.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

class IPlayListDAO;
class PlayListDAOLoader : public QObject
{
    Q_OBJECT
public:
    explicit PlayListDAOLoader(QObject *parent = 0);
    static PlayListDAOLoader *getInstance();

    virtual ~PlayListDAOLoader();

    void setPluginPath(const QString &path);
    IPlayListDAO *getPlayListDAO();

signals:
    void pluginChanged();

public slots:
    void setNewPlayListDAO(const QString &pluginName);
private:
    void initPlugin();

private:
    QList<IPlayListDAO *> mPluginList;
    QString mPluginPath;
    QString mCurrentPluginName;
    int mCurrentPluginIndex;
};

} //MusicLibrary
} //PhoenixPlayer
Q_DECLARE_INTERFACE(PhoenixPlayer::MusicLibrary::PlayListDAOLoader, "PhoenixPlayer.MusicLibrary.PlayListDAOLoader/1.0")

#endif // PLAYLISTDAOLOADER_H
