#ifndef BASEPLUGIN_H
#define BASEPLUGIN_H

#include <QObject>

#include "BaseObject.h"
#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {

class LIBPHOENIXPLAYER_EXPORT PluginProperty
{
public:
    PluginProperty();
    PluginProperty(const PluginProperty &other);

    PluginProperty(const QString &name,
                   const QString &version,
                   const QString &description,
                   bool hasAbout = false,
                   bool hasSettings = false);

    bool operator == (const PluginProperty &other) = delete;
    bool operator != (const PluginProperty &other) = delete;
    PluginProperty &operator = (const PluginProperty &other);

    bool    hasAbout;
    bool    hasSettings;
    QString name;
    QString description;
    QString version;
};

class LIBPHOENIXPLAYER_EXPORT BasePlugin : public BaseObject
{
    Q_OBJECT
public:
    enum PluginType {
        PluginTypeUndefined = 0x0,
        //为保证后续兼容,TypePlayBackend必须为第二个项
        PluginPlayBackend,              //播放后端
        PluginMusicLibraryDAO,          //音乐库存储后端
//        PluginMusicTagParser,           //音乐Tag解析
//        PluginMetadataLookup,           //metadata查询
//        PluginDecoder,                  //解码插件
//        PluginOutPut,                   //输出插件
//        PluginSpectrumGenerator,        //
        PluginDataProvider,
        PluginUserInterface
    };

    explicit BasePlugin(QObject *parent = Q_NULLPTR);
    virtual ~BasePlugin();

    virtual PluginProperty property() const  = 0;

    virtual PluginType type() const = 0;

    virtual QWidget *settingsUI() const
    {
        return Q_NULLPTR;
    }

    virtual QWidget *aboutUI() const
    {
        return Q_NULLPTR;
    }
};

} //PhoenixPlayer

Q_DECLARE_INTERFACE(PhoenixPlayer::BasePlugin, "SunRain.PhoenixPlayer.BasePluginInterface/1.0")

#endif // BASEPLUGIN_H
