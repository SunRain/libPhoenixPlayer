#ifndef COVERCIRCLEIMAGE_H
#define COVERCIRCLEIMAGE_H

#include "CircleImage.h"
#include "Common.h"

class QUrl;
namespace PhoenixPlayer {
namespace MusicLibrary {
class MusicLibraryManager;
}
namespace QmlPlugin {

class CoverCircleImage : public CircleImage
{
    Q_OBJECT
    Q_PROPERTY(QUrl defaultSource READ defaultSource WRITE setDefaultSource)
    Q_PROPERTY(bool autoChange READ autoChange WRITE setAutoChange)
    Q_PROPERTY(QString songHash READ songHash WRITE setSongHash)
public:
    explicit CoverCircleImage(CircleImage *parent = 0);

    ///
    /// \brief defaultSource 默认图标
    /// \return
    ///
    QUrl defaultSource() const;

    ///
    /// \brief 是否在歌曲改变的时候自动刷新当前图片为播放歌曲的图片,默认为false
    /// \return
    ///
    bool autoChange() const;

    QString songHash() const;

public slots:
    void setDefaultSource(const QUrl &source);
    void setAutoChange(bool autoChange);
    void setSongHash(const QString &hash);

private:
//    QUrl queryOne(const QString &hash,
//                     Common::SongMetaTags tag,
//                     bool skipDuplicates = true);
    void setImage(const QString &hash);
private:
    MusicLibrary::MusicLibraryManager *mMusicLibraryManager;
    QUrl mDefaultSource;
    bool mAutoChange;
    QString mSongHash;
};


} //QmlPlugin
} //PhoenixPlayer
#endif // COVERCIRCLEIMAGE_H
