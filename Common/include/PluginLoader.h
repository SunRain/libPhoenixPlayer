#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include <QHash>
#include <QList>

namespace PhoenixPlayer {

namespace PlayBackend {
class IPlayBackend;
}

namespace MusicLibrary {
class IPlayListDAO;
//class IMusicTagParser;
}
class PluginLoader : public QObject
{
    Q_OBJECT
    Q_ENUMS(PluginType)
public:
    //不使用强类型枚举,因为QHash不支持强类型枚举
    enum /*class*/ PluginType {
        TypeAll = 0x0,
        TypePlayBackend,
        TypePlayListDAO,
    };
    static PluginLoader *getInstance();
    explicit PluginLoader(QObject *parent = 0);
    virtual ~PluginLoader();

    void setPluginPath(PluginType type, const QString &path);

    PlayBackend::IPlayBackend *getPlayBackend();
    MusicLibrary::IPlayListDAO *getPlayListDAO();

protected:
    void initPlugins(PluginType type);
signals:
    void signalPluginChanged(PluginType type);
//    void signalPluginChanged(int type);

public slots:
     void setNewPlugin(PluginType type, const QString &newPluginName);

private:
     void initPlayBackendPlugin();
     void initPlayListDaoPlugin();
private:
      QList <PlayBackend::IPlayBackend*> mPlayBackendList;
      QList <MusicLibrary::IPlayListDAO*> mPlayListDAOList;
      QHash<PluginType, int> mCurrentPluginIndex;
      QHash<PluginType, QString> mCurrentPluginName;
      QHash<PluginType, QString> mPluginPath;
};

} //PhoenixPlayer
#endif // PLUGINLOADER_H
