#ifndef METADATALOOKUPMGRWRAPPER_H
#define METADATALOOKUPMGRWRAPPER_H

#include <QObject>
#include "IMetadataLookup.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer {
class PluginLoader;
class SongMetaData;

namespace MusicLibrary {
class MusicLibraryManager;
}

namespace MetadataLookup {

class MetadataLookupMgr;
class MetadataLookupMgrWrapper : public QObject
{
    Q_OBJECT
    DECLARE_SINGLETON_POINTER(MetadataLookupMgrWrapper)
public:
    virtual ~MetadataLookupMgrWrapper();

//#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
//    static MetadataLookupMgrWrapper *instance();
//#endif

    Q_INVOKABLE void lookupLyric(SongMetaData **data);
    Q_INVOKABLE void lookupLyric(const QString &uuid,
                                 const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupAlbumImage(SongMetaData **data);
    Q_INVOKABLE void lookupAlbumImage(const QString &uuid,
                                      const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupAlbumDescription(SongMetaData **data);
    Q_INVOKABLE void lookupAlbumDescription(const QString &uuid,
                                            const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupAlbumDate(SongMetaData **data);
    Q_INVOKABLE void lookupAlbumDate(const QString &uuid,
                                     const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupArtistImage(SongMetaData **data);
    Q_INVOKABLE void lookupArtistImage(const QString &uuid,
                                       const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupArtistDescription(SongMetaData **data);
    Q_INVOKABLE void lookupArtistDescription(const QString &uuid,
                                             const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupTrackDescription(SongMetaData **data);
    Q_INVOKABLE void lookupTrackDescription(const QString &uuid,
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
    void lookupLyricSucceed(SongMetaData **data);
    void lookupLyricFailed(SongMetaData **data);

    void lookupAlbumImageSucceed(SongMetaData **data);
    void lookupAlbumImageFailed(SongMetaData **data);

    void lookupAlbumDescriptionSucceed(SongMetaData **data);
    void lookupAlbumDescriptionFailed(SongMetaData **data);

    void lookupAlbumDateSucceed(SongMetaData **data);
    void lookupAlbumDateFailed(SongMetaData **data);

    void lookupArtistImageSucceed(SongMetaData **data);
    void lookupArtistImageFailed(SongMetaData **data);

    void lookupArtistDescriptionSucceed(SongMetaData **data);
    void lookupArtistDescriptionFailed(SongMetaData **data);

    void lookupTrackDescriptionSucceed(SongMetaData **data);
    void lookupTrackDescriptionFailed(SongMetaData **data);

    void metadataLookupFailed(SongMetaData **data);

private slots:
    void doLookupFailed(SongMetaData **data, const IMetadataLookup::LookupType &type);
    void doLookupSucceed(SongMetaData **data,
                         const IMetadataLookup::LookupType &type);

private:
    void doLookup(SongMetaData **data,
                  MetadataLookup::IMetadataLookup::LookupType type);
    void doLookupByDetail(const QString &uuid,
                          const QString &title, const QString &artist, const QString &album,
                          MetadataLookup::IMetadataLookup::LookupType type);

    void emitResult(MetadataLookup::IMetadataLookup::LookupType type,
                    SongMetaData **data,
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
