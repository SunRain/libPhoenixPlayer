#ifndef BACKENDLOADER_H
#define BACKENDLOADER_H

#include <QObject>
#include <QList>

#include "IPlayBackend.h"

namespace PhoenixPlayer{
namespace PlayBackend {

class PlayBackendLoader : public QObject
{
    Q_OBJECT
public:
    static PlayBackendLoader *getInstance();
    explicit PlayBackendLoader(QObject *parent = 0);
    void setBackendPluginPath(const QString &path);
    PlayBackend::IPlayBackend *getCurrentBackend();
signals:
    void signalPlayBackendChanged();

public slots:
    void setNewBackend(const QString &newBackendName);

private:
    virtual ~PlayBackendLoader();
    void initBackend();
private:
    QList <PlayBackend::IPlayBackend*> mBackendList;
    QString mBackendPath;
    QString mCurrentBackendName;
    int mCurrentBackendIndex;

};
} //Core
} //PhoenixPlayer
Q_DECLARE_INTERFACE(PhoenixPlayer::PlayBackend::PlayBackendLoader, "PhoenixPlayer.PlayBackend.PlayBackendLoader/1.0")

#endif // BACKENDLOADER_H
