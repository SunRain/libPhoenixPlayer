#ifndef IDATAPROVIDER_H
#define IDATAPROVIDER_H

#include <QObject>

#include "libphoenixplayer_global.h"
#include "BasePlugin.h"

namespace PhoenixPlayer {
    namespace DataProvider {

class IDataProvider : public BasePlugin
{
    Q_OBJECT
public:
    explicit IDataProvider(QObject *parent = Q_NULLPTR);
    virtual ~IDataProvider() override;

    enum SupportedType {
        SupportUndefined                = 0x0,
        SupportMusicTagParser           = 0x1,
        SupportSpectrumGenerator        = 0x2,
        SupportLookupLyrics             = 0x4,      //歌词
        SupportLookupAlbumImage         = 0x8,      //专辑封面
        SupportLookupAlbumDescription   = 0x10,     //专辑介绍
        SupportLookupAlbumDate          = 0x20,     //
        SupportLookupArtistImage        = 0x40,     //艺术家封面
        SupportLookupArtistDescription  = 0x80,     //艺术家信息
        SupportLookupTrackDescription   = 0x100,    //歌曲信息
        SupportTrackSearch              = 0x200     //Music search
    };
    Q_DECLARE_FLAGS(SupportedTypes, SupportedType)
    Q_FLAG(SupportedTypes)

    virtual bool support(SupportedTypes type) const = 0;

    virtual void stop() { }

    // BasePlugin interface
public:
    virtual PluginProperty property() const Q_DECL_OVERRIDE
    {
        return PluginProperty();
    }
    virtual PluginType type() const Q_DECL_OVERRIDE
    {
        return BasePlugin::PluginDataProvider;
    }
};

} //DataProvider
} //PhoenixPlayer

Q_DECLARE_OPERATORS_FOR_FLAGS(PhoenixPlayer::DataProvider::IDataProvider::SupportedTypes)

#endif // IDATAPROVIDER_H
