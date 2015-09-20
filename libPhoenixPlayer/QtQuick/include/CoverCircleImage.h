#ifndef COVERCIRCLEIMAGE_H
#define COVERCIRCLEIMAGE_H

#include "CircleImage.h"
#include "Common.h"

class QUrl;
namespace PhoenixPlayer {
class PlayerCore;
namespace MusicLibrary {
class MusicLibraryManager;
}
namespace QmlPlugin {

class CoverCircleImage : public CircleImage
{
    Q_OBJECT
    Q_PROPERTY(QUrl defaultSource READ defaultSource WRITE setDefaultSource)
    Q_PROPERTY(bool autoChange READ autoChange WRITE setAutoChange)
//    Q_PROPERTY(QString songHash READ songHash WRITE setSongHash)
public:
    explicit CoverCircleImage(CircleImage *parent = 0);
    virtual ~CoverCircleImage();

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

public slots:
    void setDefaultSource(QUrl arg);
    void setAutoChange(bool arg);

private slots:
    void drawImage();

private:
//    QUrl queryOne(const QString &hash,
//                     Common::SongMetaTags tag,
//                     bool skipDuplicates = true);
    void setImage(const QUrl &url);
private:
//    MusicLibrary::MusicLibraryManager *m_musicLibraryManager;
    PlayerCore *m_playCore;
//    QUrl mDefaultSource;
//    bool mAutoChange;
    //    QString mSongHash;
    QUrl m_defaultSource;
    bool m_autoChange;
};


} //QmlPlugin
} //PhoenixPlayer
#endif // COVERCIRCLEIMAGE_H
