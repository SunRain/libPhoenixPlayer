#include "MetadataLookup/MetadataLookupMgrWrapper.h"

#include <QMutex>

#include "SingletonPointer.h"
#include "PluginLoader.h"
#include "MusicLibrary/IMusicLibraryDAO.h"
#include "MusicLibrary/MusicLibraryManager.h"
#include "MusicLibrary/MusicLibraryDAOHost.h"
#include "MetadataLookup/MetadataLookupMgr.h"
#include "MetadataLookup/MetadataLookupHost.h"

namespace PhoenixPlayer {
namespace MetadataLookup {

using namespace PhoenixPlayer::MusicLibrary;

MetadataLookupMgrWrapper::MetadataLookupMgrWrapper(QObject *parent) :
    QObject(parent)
{
    m_pluginLoader = PluginLoader::instance ();
    m_musicLibraryManager = MusicLibraryManager::instance ();

    m_lookupMgr = new MetadataLookupMgr(this);

    //SongMetaData **data, const IMetadataLookup::LookupType &type
    connect (m_lookupMgr, &MetadataLookupMgr::lookupFailed,
             this, &MetadataLookupMgrWrapper::doLookupFailed);
    //SongMetaData **data, const IMetadataLookup::LookupType &type
    connect (m_lookupMgr, &MetadataLookupMgr::lookupSucceed,
             this, &MetadataLookupMgrWrapper::doLookupSucceed);

    connect (m_lookupMgr,
             &MetadataLookupMgr::queueFinished,
             [&] {
        qDebug()<<Q_FUNC_INFO<<"========>>> queueFinished <<<<========";
    });
}

MetadataLookupMgrWrapper::~MetadataLookupMgrWrapper()
{
    if (m_lookupMgr->isRunning ()) {
        m_lookupMgr->quit ();
        m_lookupMgr->wait (3*1000);
        m_lookupMgr->deleteLater ();
        m_lookupMgr = nullptr;
    }
    if (!m_failList.isEmpty ())
        m_failList.clear ();

}

void MetadataLookupMgrWrapper::lookupLyric(AudioMetaObject **data)
{
    this->doLookup (data, IMetadataLookup::TypeLyrics);
}

void MetadataLookupMgrWrapper::lookupLyric(const QString &uuid, const QString &title,
                                           const QString &artist, const QString &album)
{
    this->doLookupByDetail (uuid, title, artist, album, IMetadataLookup::TypeLyrics);
}

void MetadataLookupMgrWrapper::lookupAlbumImage(AudioMetaObject **data)
{
    this->doLookup (data, IMetadataLookup::TypeAlbumImage);
}

void MetadataLookupMgrWrapper::lookupAlbumImage(const QString &uuid, const QString &title,
                                                const QString &artist, const QString &album)
{
    this->doLookupByDetail (uuid, title, artist, album, IMetadataLookup::TypeAlbumImage);
}

void MetadataLookupMgrWrapper::lookupAlbumDescription(AudioMetaObject **data)
{
    this->doLookup (data, IMetadataLookup::TypeAlbumDescription);
}

void MetadataLookupMgrWrapper::lookupAlbumDescription(const QString &uuid, const QString &title,
                                                      const QString &artist, const QString &album)
{
    this->doLookupByDetail (uuid, title, artist, album, IMetadataLookup::TypeAlbumDescription);
}

void MetadataLookupMgrWrapper::lookupAlbumDate(AudioMetaObject **data)
{
    this->doLookup (data, IMetadataLookup::TypeAlbumDate);
}

void MetadataLookupMgrWrapper::lookupAlbumDate(const QString &uuid, const QString &title,
                                               const QString &artist, const QString &album)
{
    this->doLookupByDetail (uuid, title, artist, album, IMetadataLookup::TypeAlbumDate);
}

void MetadataLookupMgrWrapper::lookupArtistImage(AudioMetaObject **data)
{
    this->doLookup (data, IMetadataLookup::TypeArtistImage);
}

void MetadataLookupMgrWrapper::lookupArtistImage(const QString &uuid, const QString &title,
                                                 const QString &artist, const QString &album)
{
    this->doLookupByDetail (uuid, title, artist, album, IMetadataLookup::TypeArtistImage);
}

void MetadataLookupMgrWrapper::lookupArtistDescription(AudioMetaObject **data)
{
    this->doLookup (data, IMetadataLookup::TypeArtistDescription);
}

void MetadataLookupMgrWrapper::lookupArtistDescription(const QString &uuid, const QString &title,
                                                       const QString &artist, const QString &album)
{
    this->doLookupByDetail (uuid, title, artist, album, IMetadataLookup::TypeArtistDescription);
}

void MetadataLookupMgrWrapper::lookupTrackDescription(AudioMetaObject **data)
{
    this->doLookup (data, IMetadataLookup::TypeTrackDescription);
}

void MetadataLookupMgrWrapper::lookupTrackDescription(const QString &uuid, const QString &title,
                                                      const QString &artist, const QString &album)
{
    this->doLookupByDetail (uuid, title, artist, album, IMetadataLookup::TypeTrackDescription);
}

//SongMetaData **data, const IMetadataLookup::LookupType &type
void MetadataLookupMgrWrapper::doLookupFailed(AudioMetaObject **data, const IMetadataLookup::LookupType &type)
{
    this->emitResult (type, data, QString(), false);
}

//SongMetaData **data, const IMetadataLookup::LookupType &type
void MetadataLookupMgrWrapper::doLookupSucceed(AudioMetaObject **data,
                                               const IMetadataLookup::LookupType &type)
{
    qDebug()<<Q_FUNC_INFO;

    if (m_pluginLoader) {
        qDebug()<<Q_FUNC_INFO<<"had mPluginLoader";
//        IMusicLibraryDAO *dao = m_pluginLoader->getCurrentLibraryDAO ();
//        MusicLibraryDAOHost *host = m_pluginLoader->curDAOHost ();
        MusicLibraryDAOHost *host = m_pluginLoader->curDAOHost ();
        if (host) {
            IMusicLibraryDAO *dao = host->instance<IMusicLibraryDAO>();
            if (dao)
                dao->updateMetaData (data, true);
        }
    }

    this->emitResult (type, data, QString(), true);
}

void MetadataLookupMgrWrapper::doLookup(AudioMetaObject **data, IMetadataLookup::LookupType type)
{
    if (!data || !*data) {
        qWarning()<<Q_FUNC_INFO<<"We can't lookup with empty one";
        return;
    }
    bool skip = false;
    foreach (FailNode node, m_failList) {
        if ((node.hash == (*data)->hash ()) && (node.type == type)) {
            qDebug()<<Q_FUNC_INFO<<QString("Current hash [%1] with type [%2] has in fail list")
                      .arg((*data)->hash ()).arg(type);
            skip = true;
            break;
        }
    }
    if (skip) {
        this->emitResult (type, data, QString("Had failed in previous queue,ignore it."), false);
        return;
    }

//    SongMetaData d;
//    for (int i = (int)Common::SongMetaTags::E_FirstFlag + 1;
//         i < (int)Common::SongMetaTags::E_LastFlag;
//         ++i) {
//        QString str = m_musicLibraryManager->queryOne(songHash, Common::SongMetaTags(i), true);
//        if (!str.isEmpty())
//            d.setMeta (Common::SongMetaTags(i), str);
//    }
    m_lookupMgr->lookup (data, type);
}

void MetadataLookupMgrWrapper::doLookupByDetail(const QString &uuid, const QString &title, const QString &artist, const QString &album, MetadataLookup::IMetadataLookup::LookupType type)
{
    //TODO dummy

//    if (uuid.isEmpty ()) {
//        qWarning()<<Q_FUNC_INFO<<"We can't lookup with empty uuid";
//        return;
//    }
//    SongMetaData d;
//    for (int i = (int)Common::SongMetaTags::E_FirstFlag + 1;
//         i < (int)Common::SongMetaTags::E_LastFlag;
//         ++i) {
//        if (i == (int)Common::SongMetaTags::E_TrackTitle && !title.isEmpty ()) {
//            d.setMeta (Common::SongMetaTags(i), title);
//        } else if (i == (int)Common::SongMetaTags::E_ArtistName && !artist.isEmpty ()) {
//            d.setMeta (Common::SongMetaTags(i), artist);
//        } else if (i == (int)Common::SongMetaTags::E_AlbumName && !album.isEmpty ()) {
//            d.setMeta (Common::SongMetaTags(i), album);
//        }
//    }
//    m_lookupMgr->lookup (&d, type);
}

void MetadataLookupMgrWrapper::emitResult(MetadataLookup::IMetadataLookup::LookupType type,
                                          AudioMetaObject **data, const QString &result, bool succeed)
{
    qDebug()<<Q_FUNC_INFO;
    if (!succeed) {
        FailNode node;
        node.hash = (*data)->hash ();
        node.type = type;
        if (!m_failList.contains(node))
            m_failList.append(node);
    }
    //TODO 添加其他类型的emit
    switch (type) {
    case IMetadataLookup::TypeLyrics: {
        if (succeed)
            emit lookupLyricSucceed (data);
        else
            emit lookupLyricFailed (data);
        break;
    }
    case IMetadataLookup::TypeAlbumDescription: {
        if (succeed)
            emit lookupAlbumDescriptionSucceed (data);
        else
            emit lookupAlbumDescriptionFailed (data);
        break;
    }
    case IMetadataLookup::TypeAlbumImage: {
        if (succeed)
            emit lookupAlbumImageSucceed (data);
        else
            emit lookupAlbumImageFailed (data);
        break;
    }
    case IMetadataLookup::TypeAlbumDate: {
        if (succeed)
            emit lookupAlbumDateSucceed (data);
        else
            emit lookupAlbumDateFailed (data);
        break;
    }
    case IMetadataLookup::TypeArtistDescription: {
        if (succeed)
            emit lookupArtistDescriptionSucceed (data);
        else
            emit lookupArtistDescriptionFailed (data);
        break;
    }
    case IMetadataLookup::TypeArtistImage: {
        if (succeed)
            emit lookupArtistImageSucceed (data);
        else
            emit lookupArtistImageFailed (data);
        break;
    }
    case IMetadataLookup::TypeTrackDescription: {
        if (succeed)
            emit lookupTrackDescriptionSucceed (data);
        else
            emit lookupTrackDescriptionFailed (data);
        break;
    }
    case IMetadataLookup::TypeUndefined:
    default: {
        if (!succeed)
            emit metadataLookupFailed (data);
        break;
    }
    }
}
} //MetadataLookup
} //PhoenixPlayer
