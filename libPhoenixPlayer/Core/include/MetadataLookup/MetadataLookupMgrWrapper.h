#ifndef METADATALOOKUPMGRWRAPPER_H
#define METADATALOOKUPMGRWRAPPER_H

#include <QObject>
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
class MetadataLookupMgrWrapper : public QObject
{
    Q_OBJECT
    DECLARE_SINGLETON_POINTER(MetadataLookupMgrWrapper)
public:
    virtual ~MetadataLookupMgrWrapper();

//#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
//    static MetadataLookupMgrWrapper *instance();
//#endif

    Q_INVOKABLE void lookupLyric(AudioMetaObject **data);
    Q_INVOKABLE void lookupLyric(const QString &uuid,
                                 const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupAlbumImage(AudioMetaObject **data);
    Q_INVOKABLE void lookupAlbumImage(const QString &uuid,
                                      const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupAlbumDescription(AudioMetaObject **data);
    Q_INVOKABLE void lookupAlbumDescription(const QString &uuid,
                                            const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupAlbumDate(AudioMetaObject **data);
    Q_INVOKABLE void lookupAlbumDate(const QString &uuid,
                                     const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupArtistImage(AudioMetaObject **data);
    Q_INVOKABLE void lookupArtistImage(const QString &uuid,
                                       const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupArtistDescription(AudioMetaObject **data);
    Q_INVOKABLE void lookupArtistDescription(const QString &uuid,
                                             const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupTrackDescription(AudioMetaObject **data);
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
    void lookupLyricSucceed(AudioMetaObject **data);
    void lookupLyricFailed(AudioMetaObject **data);

    void lookupAlbumImageSucceed(AudioMetaObject **data);
    void lookupAlbumImageFailed(AudioMetaObject **data);

    void lookupAlbumDescriptionSucceed(AudioMetaObject **data);
    void lookupAlbumDescriptionFailed(AudioMetaObject **data);

    void lookupAlbumDateSucceed(AudioMetaObject **data);
    void lookupAlbumDateFailed(AudioMetaObject **data);

    void lookupArtistImageSucceed(AudioMetaObject **data);
    void lookupArtistImageFailed(AudioMetaObject **data);

    void lookupArtistDescriptionSucceed(AudioMetaObject **data);
    void lookupArtistDescriptionFailed(AudioMetaObject **data);

    void lookupTrackDescriptionSucceed(AudioMetaObject **data);
    void lookupTrackDescriptionFailed(AudioMetaObject **data);

    void metadataLookupFailed(AudioMetaObject **data);

private slots:
    void doLookupFailed(AudioMetaObject **data, const IMetadataLookup::LookupType &type);
    void doLookupSucceed(AudioMetaObject **data,
                         const IMetadataLookup::LookupType &type);

private:
    void doLookup(AudioMetaObject **data,
                  MetadataLookup::IMetadataLookup::LookupType type);
    void doLookupByDetail(const QString &uuid,
                          const QString &title, const QString &artist, const QString &album,
                          MetadataLookup::IMetadataLookup::LookupType type);

    void emitResult(MetadataLookup::IMetadataLookup::LookupType type,
                    AudioMetaObject **data,
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
