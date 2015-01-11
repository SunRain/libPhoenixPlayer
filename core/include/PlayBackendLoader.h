#ifndef BACKENDLOADER_H
#define BACKENDLOADER_H

#include <QObject>
#include <QList>

#include "IPlayBackend.h"

namespace PhoenixPlayerCore {

class PlayBackendLoader : public QObject
{
    Q_OBJECT
public:
    PlayBackendLoader *getInstance();
    explicit PlayBackendLoader(QObject *parent = 0);
    void setBackendPluginPath(const QString &path);
    IPlayBackend *getCurrentBackend();
//    BasePlayBackendInterface *getCurrentBackend();
//    BasePlayBackendInterface *getBackend(const QString &backendName);

signals:
    void signalPlayBackendChanged(/*QString newPlayBackendName*/);

public slots:
    void setNewBackend(const QString &newBackendName);

private:
//    explicit PlayBackendLoader(QObject *parent = 0);
    virtual ~PlayBackendLoader();
    void initBackend();
private:
    QList <IPlayBackend*> mBackendList;
    QString mBackendPath;
    QString mCurrentBackendName;
    int mCurrentBackendIndex;

};
}
Q_DECLARE_INTERFACE(PhoenixPlayerCore::PlayBackendLoader, "PhoenixPlayerCore.PlayBackendLoader/1.0")

#endif // BACKENDLOADER_H
