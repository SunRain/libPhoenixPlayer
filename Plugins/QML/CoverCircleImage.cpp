
#include "CoverCircleImage.h"

#include <QUrl>

#include "MusicLibraryManager.h"

namespace PhoenixPlayer {
using namespace MusicLibrary;
namespace QmlPlugin {

CoverCircleImage::CoverCircleImage(CircleImage *parent)
    :CircleImage(parent)
{
#ifdef SAILFISH_OS
    mMusicLibraryManager = MusicLibraryManager::instance();
#endif

    mAutoChange = false;
    connect(mMusicLibraryManager,
            &MusicLibraryManager::playingSongChanged,
            [this]() {
        if (autoChange()) {
            mSongHash = mMusicLibraryManager->playingSongHash();
            setImage(mSongHash);
        }
    });
}

QUrl CoverCircleImage::defaultSource() const
{
    return mDefaultSource;
}

bool CoverCircleImage::autoChange() const
{
    return mAutoChange;
}

QString CoverCircleImage::songHash() const
{
    return mSongHash;
}

void CoverCircleImage::setDefaultSource(const QUrl &source)
{
    if (source.isEmpty() || !source.isValid() || mDefaultSource == source)
        return;
    qDebug()<<"setDefaultSource to "<<source;
    mDefaultSource = source;
    this->setSource(mDefaultSource);
}

void CoverCircleImage::setAutoChange(bool autoChange)
{
    mAutoChange = autoChange;
}

void CoverCircleImage::setSongHash(const QString &hash)
{
    if (hash.isEmpty() || mSongHash == hash)
        return;
    mSongHash = hash;
    qDebug()<<__FUNCTION__<<" set image by hash "<<mSongHash;
    setImage(mSongHash);
}

//QUrl CoverCircleImage::queryOne(const QString &hash,
//                                   Common::SongMetaTags tag,
//                                   bool skipDuplicates)
//{
//    if (hash.isEmpty())
//        return QUrl();
//    QStringList list = mMusicLibraryManager
//            ->querySongMetaElement(tag, hash, skipDuplicates);
//    if (list.isEmpty())
//        return QUrl();
//    return QUrl(list.first());
//}

void CoverCircleImage::setImage(const QString &hash)
{
//    QUrl uri = queryOne(hash, Common::E_CoverArtMiddle);
//    if (uri.isEmpty() || !uri.isValid())
//        uri = queryOne(hash, Common::E_CoverArtLarge);
//    if (uri.isEmpty() || !uri.isValid())
//        uri = queryOne(hash, Common::E_CoverArtSmall);
//    if (uri.isEmpty() || !uri.isValid())
//        uri = queryOne(hash, Common::E_AlbumImageUrl);
//    if (uri.isEmpty()|| !uri.isValid())
//        uri = queryOne(hash, Common::E_ArtistImageUri);
//    if (uri.isEmpty() || !uri.isValid())
//        this->setSource(mDefaultSource);
//    else
//        this->setSource(uri);

    QString str = mMusicLibraryManager->querySongImageUri(hash);
    if (str.isEmpty())
        this->setSource(mDefaultSource);
    else
        this->setSource(QUrl(str));
}



} //QmlPlugin
} //PhoenixPlayer
