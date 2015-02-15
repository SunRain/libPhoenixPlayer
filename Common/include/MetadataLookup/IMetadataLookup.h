#ifndef IMETADATALOOKUP
#define IMETADATALOOKUP

#include <QObject>
#include <QDebug>
class QByteArray;

namespace PhoenixPlayer{
class SongMetaData;
namespace MetadataLookup {

class IMetadataLookup : public QObject {
    Q_OBJECT
public:
    enum LookupType {
        TypeUndefined = 0x0,
        TypeLyrics,             //歌词
        TypeAlbumImage,         //专辑封面
        TypeAlbumDescription,   //专辑介绍
        TypeArtistImage,        //艺术家封面
        TypeArtistDescription,  //艺术家信息
        TypeSongDescription     //歌曲信息
    };

    explicit IMetadataLookup(QObject *parent = 0) : QObject(parent) {
        mType = LookupType::TypeUndefined;
    }

    virtual ~IMetadataLookup() {
        qDebug()<<__FUNCTION__;
    }

    virtual QString getPluginName() = 0;
    virtual QString getPluginVersion() = 0;
    virtual QString getDescription() = 0;

    ///
    /// \brief supportLookup 查询当前插件是否支持某一个枚举类的查询
    /// \param type
    /// \return
    ///
    virtual bool supportLookup(LookupType type = LookupType::TypeUndefined) = 0;

    ///
    /// \brief setCurrentLookupFlag 设置当前插件的查询状态,便于外部调用查看插件查询类型
    /// \param type
    ///
    void setCurrentLookupFlag(LookupType type = LookupType::TypeUndefined) {
        mType = type;
    }

    ///
    /// \brief currentLookupFlag
    /// \return 得到当前插件查询的类型
    ///
    LookupType currentLookupFlag() {
        return mType;
    }

    virtual void lookup(SongMetaData *meta) = 0;

signals:
    void lookupFailed();
    void lookupSucceed(QByteArray result);

private:
    LookupType mType;
};

} //MetadataLookup
} //PhoenixPlayer

Q_DECLARE_INTERFACE(PhoenixPlayer::MetadataLookup::IMetadataLookup, "PhoenixPlayer.MetadataLookup.IMetadataLookup/1.0")

#endif // IMETADATALOOKUP

