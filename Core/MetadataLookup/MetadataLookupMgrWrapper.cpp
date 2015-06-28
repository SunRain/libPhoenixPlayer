#include "MetadataLookup/MetadataLookupMgrWrapper.h"

#include <QMutex>

#include "SingletonPointer.h"
#include "PluginLoader.h"
#include "MusicLibrary/IPlayListDAO.h"
#include "MusicLibrary/MusicLibraryManager.h"
#include "MetadataLookup/MetadataLookupMgr.h"

namespace PhoenixPlayer {
namespace MetadataLookup {

using namespace PhoenixPlayer::MusicLibrary;

MetadataLookupMgrWrapper::MetadataLookupMgrWrapper(QObject *parent) :
    QObject(parent)
{
#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    mPluginLoader = PluginLoader::instance();
    mMusicLibraryManager = MusicLibraryManager::instance();
#else
    mPluginLoader = SingletonPointer<PluginLoader>::instance ();
    mMusicLibraryManager = SingletonPointer<MusicLibraryManager>::instance ();
#endif

    mLookupMgr = new MetadataLookupMgr(this);

    connect (mLookupMgr, &MetadataLookupMgr::lookupFailed,
             this, &MetadataLookupMgrWrapper::doLookupFailed);
    connect (mLookupMgr, &MetadataLookupMgr::lookupSucceed,
             this, &MetadataLookupMgrWrapper::doLookupSucceed);

    connect (mLookupMgr,
             &MetadataLookupMgr::queueFinished,
             [&] {
        qDebug()<<Q_FUNC_INFO<<"========>>> queueFinished <<<<========";
    });
}

MetadataLookupMgrWrapper::~MetadataLookupMgrWrapper()
{

}

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
MetadataLookupMgrWrapper *MetadataLookupMgrWrapper::instance()
{
    static QMutex mutex;
    static QScopedPointer<MetadataLookupMgrWrapper> scp;
    if (Q_UNLIKELY(scp.isNull())) {
        mutex.lock();
        qDebug()<<Q_FUNC_INFO<<"==== start new";
        scp.reset(new MetadataLookupMgrWrapper(0));
        mutex.unlock();
    }
    qDebug()<<Q_FUNC_INFO<<"==== return old";
    return scp.data();
}
#endif

void MetadataLookupMgrWrapper::lookupLyric(const QString &songHash)
{
    this->doLookupByHash (songHash, IMetadataLookup::TypeLyrics);
}

void MetadataLookupMgrWrapper::lookupLyric(const QString &uuid, const QString &title,
                                           const QString &artist, const QString &album)
{
    this->doLookupByDetail (uuid, title, artist, album, IMetadataLookup::TypeLyrics);
}

void MetadataLookupMgrWrapper::lookupAlbumImage(const QString &songHash)
{
    this->doLookupByHash (songHash, IMetadataLookup::TypeAlbumImage);
}

void MetadataLookupMgrWrapper::lookupAlbumImage(const QString &uuid, const QString &title,
                                                const QString &artist, const QString &album)
{
    this->doLookupByDetail (uuid, title, artist, album, IMetadataLookup::TypeAlbumImage);
}

void MetadataLookupMgrWrapper::lookupAlbumDescription(const QString &songHash)
{
    this->doLookupByHash (songHash, IMetadataLookup::TypeAlbumDescription);
}

void MetadataLookupMgrWrapper::lookupAlbumDescription(const QString &uuid, const QString &title,
                                                      const QString &artist, const QString &album)
{
    this->doLookupByDetail (uuid, title, artist, album, IMetadataLookup::TypeAlbumDescription);
}

void MetadataLookupMgrWrapper::lookupAlbumDate(const QString &songHash)
{
    this->doLookupByHash (songHash, IMetadataLookup::TypeAlbumDate);
}

void MetadataLookupMgrWrapper::lookupAlbumDate(const QString &uuid, const QString &title,
                                               const QString &artist, const QString &album)
{
    this->doLookupByDetail (uuid, title, artist, album, IMetadataLookup::TypeAlbumDate);
}

void MetadataLookupMgrWrapper::lookupArtistImage(const QString &songHash)
{
    this->doLookupByHash (songHash, IMetadataLookup::TypeArtistImage);
}

void MetadataLookupMgrWrapper::lookupArtistImage(const QString &uuid, const QString &title,
                                                 const QString &artist, const QString &album)
{
    this->doLookupByDetail (uuid, title, artist, album, IMetadataLookup::TypeArtistImage);
}

void MetadataLookupMgrWrapper::lookupArtistDescription(const QString &songHash)
{
    this->doLookupByHash (songHash, IMetadataLookup::TypeArtistDescription);
}

void MetadataLookupMgrWrapper::lookupArtistDescription(const QString &uuid, const QString &title,
                                                       const QString &artist, const QString &album)
{
    this->doLookupByDetail (uuid, title, artist, album, IMetadataLookup::TypeArtistDescription);
}

void MetadataLookupMgrWrapper::lookupTrackDescription(const QString &songHash)
{
    this->doLookupByHash (songHash, IMetadataLookup::TypeTrackDescription);
}

void MetadataLookupMgrWrapper::lookupTrackDescription(const QString &uuid, const QString &title,
                                                      const QString &artist, const QString &album)
{
    this->doLookupByDetail (uuid, title, artist, album, IMetadataLookup::TypeTrackDescription);
}

void MetadataLookupMgrWrapper::doLookupFailed(const QString &songHash,
                                              const IMetadataLookup::LookupType &type)
{
    this->emitResult (type, songHash, QString(), false);
}

void MetadataLookupMgrWrapper::doLookupSucceed(const QString &songHash, const QByteArray &result,
                                               const IMetadataLookup::LookupType &type)
{
    qDebug()<<Q_FUNC_INFO;

    SongMetaData d;
    d.setMeta (Common::SongMetaTags::E_Hash, songHash);

    switch (type) {
    case IMetadataLookup::TypeAlbumDate:
        d.setMeta (Common::E_AlbumYear, result);
        break;
    case IMetadataLookup::TypeAlbumDescription:
        d.setMeta (Common::E_AlbumDescription, result);
        break;
    case IMetadataLookup::TypeAlbumImage:
        d.setMeta (Common::E_AlbumImageUrl, result);
        break;
    case  IMetadataLookup::TypeArtistDescription:
        d.setMeta (Common::E_ArtistDescription, result);
        break;
    case IMetadataLookup::TypeArtistImage:
        d.setMeta (Common::E_ArtistImageUri, result);
        break;
    case IMetadataLookup::TypeLyrics:
        d.setMeta (Common::E_Lyrics, result);
        break;
    case IMetadataLookup::TypeTrackDescription:
        d.setMeta (Common::E_SongDescription, result);
        break;
    default:
        break;
    }

    if (mPluginLoader) {
        qDebug()<<Q_FUNC_INFO<<"had mPluginLoader";
        IPlayListDAO *dao = mPluginLoader->getCurrentPlayListDAO ();
        if (dao)
            dao->updateMetaData (&d, true);
    }

    this->emitResult (type, songHash, QString(result), true);
}

void MetadataLookupMgrWrapper::doLookupByHash(const QString &songHash, MetadataLookup::IMetadataLookup::LookupType type)
{
    if (songHash.isEmpty ()) {
        qWarning()<<Q_FUNC_INFO<<"We can't lookup with empty song hash";
        return;
    }
    SongMetaData d;
    for (int i = (int)Common::SongMetaTags::E_FirstFlag + 1;
         i < (int)Common::SongMetaTags::E_LastFlag;
         ++i) {
        QString str = mMusicLibraryManager->queryOne(songHash, Common::SongMetaTags(i), true);
        if (!str.isEmpty())
            d.setMeta (Common::SongMetaTags(i), str);
    }
    mLookupMgr->lookup (&d, type);
}

void MetadataLookupMgrWrapper::doLookupByDetail(const QString &uuid, const QString &title, const QString &artist, const QString &album, MetadataLookup::IMetadataLookup::LookupType type)
{
    if (uuid.isEmpty ()) {
        qWarning()<<Q_FUNC_INFO<<"We can't lookup with empty uuid";
        return;
    }
    SongMetaData d;
    for (int i = (int)Common::SongMetaTags::E_FirstFlag + 1;
         i < (int)Common::SongMetaTags::E_LastFlag;
         ++i) {
        if (i == (int)Common::SongMetaTags::E_SongTitle && !title.isEmpty ()) {
            d.setMeta (Common::SongMetaTags(i), title);
        } else if (i == (int)Common::SongMetaTags::E_ArtistName && !artist.isEmpty ()) {
            d.setMeta (Common::SongMetaTags(i), artist);
        } else if (i == (int)Common::SongMetaTags::E_AlbumName && !album.isEmpty ()) {
            d.setMeta (Common::SongMetaTags(i), album);
        }
    }
    mLookupMgr->lookup (&d, type);
}

void MetadataLookupMgrWrapper::emitResult(MetadataLookup::IMetadataLookup::LookupType type,
                                          const QString &hash, const QString &result, bool succeed)
{
    qDebug()<<Q_FUNC_INFO;
    //TODO 添加其他类型的emit
    switch (type) {
    case IMetadataLookup::TypeLyrics: {
        if (succeed)
            emit lookupLyricSucceed (hash, result);
        else
            emit lookupLyricFailed (hash);
        break;
    }
    case IMetadataLookup::TypeAlbumDescription: {
        if (succeed)
            emit lookupAlbumDescriptionSucceed (hash, result);
        else
            emit lookupAlbumDescriptionFailed (hash);
        break;
    }
    case IMetadataLookup::TypeAlbumImage: {
        if (succeed)
            emit lookupAlbumImageSucceed (hash, result);
        else
            emit lookupAlbumImageFailed (hash);
        break;
    }
    case IMetadataLookup::TypeAlbumDate: {
        if (succeed)
            emit lookupAlbumDateSucceed (hash, result);
        else
            emit lookupAlbumDateFailed (hash);
        break;
    }
    case IMetadataLookup::TypeArtistDescription: {
        if (succeed)
            emit lookupArtistDescriptionSucceed (hash, result);
        else
            emit lookupArtistDescriptionFailed (hash);
        break;
    }
    case IMetadataLookup::TypeArtistImage: {
        if (succeed)
            emit lookupArtistImageSucceed (hash, result);
        else
            emit lookupArtistImageFailed (hash);
        break;
    }
    case IMetadataLookup::TypeTrackDescription: {
        if (succeed)
            emit lookupTrackDescriptionSucceed (hash, result);
        else
            emit lookupTrackDescriptionFailed (hash);
        break;
    }
    case IMetadataLookup::TypeUndefined:
    default: {
        if (!succeed)
            emit metadataLookupFailed (hash);
        break;
    }
    }
}
} //MetadataLookup
} //PhoenixPlayer
