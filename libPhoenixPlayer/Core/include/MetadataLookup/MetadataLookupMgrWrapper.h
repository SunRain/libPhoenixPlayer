#ifndef METADATALOOKUPMGRWRAPPER_H
#define METADATALOOKUPMGRWRAPPER_H

#include <QObject>

#include "libphoenixplayer_global.h"
#include "IMetadataLookup.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer {
class PluginLoader;
class AudioMetaObject;

namespace MusicLibrary {
class MusicLibraryManager;
}

namespace MetadataLookup {

class MetadataLookupMgr;
class LIBPHOENIXPLAYER_EXPORT MetadataLookupMgrWrapper : public QObject
{
    Q_OBJECT
    DECLARE_SINGLETON_POINTER(MetadataLookupMgrWrapper)
public:
    virtual ~MetadataLookupMgrWrapper();

    void lookupLyric(const AudioMetaObject &data);
    void lookupLyric(const QString &uuid,
                                 const QString &title, const QString &artist, const QString &album);

    void lookupAlbumImage(const AudioMetaObject &data);
    void lookupAlbumImage(const QString &uuid,
                                      const QString &title, const QString &artist, const QString &album);

    void lookupAlbumDescription(const AudioMetaObject &data);
    void lookupAlbumDescription(const QString &uuid,
                                            const QString &title, const QString &artist, const QString &album);

    void lookupAlbumDate(const AudioMetaObject &data);
    void lookupAlbumDate(const QString &uuid,
                                     const QString &title, const QString &artist, const QString &album);

    void lookupArtistImage(const AudioMetaObject &data);
    void lookupArtistImage(const QString &uuid,
                                       const QString &title, const QString &artist, const QString &album);

    void lookupArtistDescription(const AudioMetaObject &data);
    void lookupArtistDescription(const QString &uuid,
                                             const QString &title, const QString &artist, const QString &album);

    void lookupTrackDescription(const AudioMetaObject &data);
    void lookupTrackDescription(const QString &uuid,
                                            const QString &title, const QString &artist, const QString &album);

protected:
//    explicit MetadataLookupMgrWrapper(QObject *parent = 0);
    struct FailNode {
        QString hash;
        IMetadataLookup::LookupType type;
        bool operator ==(const FailNode &other) const {
            return (this->hash == other.hash) && (this->type == other.type);
        }
    };
signals:
    void lookupLyricSucceed(const AudioMetaObject &data);
    void lookupLyricFailed(const AudioMetaObject &data);

    void lookupAlbumImageSucceed(const AudioMetaObject &data);
    void lookupAlbumImageFailed(const AudioMetaObject &data);

    void lookupAlbumDescriptionSucceed(const AudioMetaObject &data);
    void lookupAlbumDescriptionFailed(const AudioMetaObject &data);

    void lookupAlbumDateSucceed(const AudioMetaObject &data);
    void lookupAlbumDateFailed(const AudioMetaObject &data);

    void lookupArtistImageSucceed(const AudioMetaObject &data);
    void lookupArtistImageFailed(const AudioMetaObject &data);

    void lookupArtistDescriptionSucceed(const AudioMetaObject &data);
    void lookupArtistDescriptionFailed(const AudioMetaObject &data);

    void lookupTrackDescriptionSucceed(const AudioMetaObject &data);
    void lookupTrackDescriptionFailed(const AudioMetaObject &data);

    void metadataLookupFailed(const AudioMetaObject &data);

private slots:
    void doLookupFailed(const AudioMetaObject &data, const IMetadataLookup::LookupType &type);
    void doLookupSucceed(const AudioMetaObject &data,
                         const IMetadataLookup::LookupType &type);

private:
    void doLookup(const AudioMetaObject &data,
                  MetadataLookup::IMetadataLookup::LookupType type);
    void doLookupByDetail(const QString &uuid,
                          const QString &title, const QString &artist, const QString &album,
                          MetadataLookup::IMetadataLookup::LookupType type);

    void emitResult(MetadataLookup::IMetadataLookup::LookupType type,
                    const AudioMetaObject &data,
                    const QString &result,
                    bool succeed);
private:
    QList<FailNode> m_failList;
    MetadataLookupMgr *m_lookupMgr;
    PluginLoader *m_pluginLoader;
    MusicLibrary::MusicLibraryManager *m_musicLibraryManager;
};
} //MetadataLookup
} //PhoenixPlayer
#endif // METADATALOOKUPMGRWRAPPER_H
