#ifndef METADATALOOKUPMGRWRAPPER_H
#define METADATALOOKUPMGRWRAPPER_H

#include <QObject>
#include "IMetadataLookup.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer {
class PluginLoader;
//class PluginHost;
//class Settings;
//class Util;

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

    Q_INVOKABLE void lookupLyric(const QString &songHash);
    Q_INVOKABLE void lookupLyric(const QString &uuid,
                                 const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupAlbumImage(const QString &songHash);
    Q_INVOKABLE void lookupAlbumImage(const QString &uuid,
                                      const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupAlbumDescription(const QString &songHash);
    Q_INVOKABLE void lookupAlbumDescription(const QString &uuid,
                                            const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupAlbumDate(const QString &songHash);
    Q_INVOKABLE void lookupAlbumDate(const QString &uuid,
                                     const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupArtistImage(const QString &songHash);
    Q_INVOKABLE void lookupArtistImage(const QString &uuid,
                                       const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupArtistDescription(const QString &songHash);
    Q_INVOKABLE void lookupArtistDescription(const QString &uuid,
                                             const QString &title, const QString &artist, const QString &album);

    Q_INVOKABLE void lookupTrackDescription(const QString &songHash);
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
    void lookupLyricSucceed(const QString &hash, const QString &result);
    void lookupLyricFailed(const QString &hash);

    void lookupAlbumImageSucceed(const QString &hash, const QString &result);
    void lookupAlbumImageFailed(const QString &hash);

    void lookupAlbumDescriptionSucceed(const QString &hash, const QString &result);
    void lookupAlbumDescriptionFailed(const QString &hash);

    void lookupAlbumDateSucceed(const QString &hash, const QString &result);
    void lookupAlbumDateFailed(const QString &hash);

    void lookupArtistImageSucceed(const QString &hash, const QString &result);
    void lookupArtistImageFailed(const QString &hash);

    void lookupArtistDescriptionSucceed(const QString &hash, const QString &result);
    void lookupArtistDescriptionFailed(const QString &hash);

    void lookupTrackDescriptionSucceed(const QString &hash, const QString &result);
    void lookupTrackDescriptionFailed(const QString &hash);

    void metadataLookupFailed(const QString &hash);

private slots:
    void doLookupFailed(const QString &songHash, const IMetadataLookup::LookupType &type);
    void doLookupSucceed(const QString &songHash,
                         const QByteArray &result,
                         const IMetadataLookup::LookupType &type);

private:
    void doLookupByHash(const QString &songHash,
                        MetadataLookup::IMetadataLookup::LookupType type);
    void doLookupByDetail(const QString &uuid,
                          const QString &title, const QString &artist, const QString &album,
                          MetadataLookup::IMetadataLookup::LookupType type);

    void emitResult(MetadataLookup::IMetadataLookup::LookupType type,
                    const QString &hash,
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
