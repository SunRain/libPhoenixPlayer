#ifndef PLAYLISTMETAMGRINTERNAL_H
#define PLAYLISTMETAMGRINTERNAL_H

#include <QObject>
#include <QList>
#include <QSharedPointer>

#include "PlayerCore/PlayListMeta.h"
#include "PlayerCore/PlayListMetaMgr.h"

namespace PhoenixPlayer {

class PPSettingsInternal;

class PlayListMetaMgrInternal : public QObject
{
    Q_OBJECT
public:
    explicit PlayListMetaMgrInternal(QSharedPointer<PPSettingsInternal> set,
                                     QObject *parent = Q_NULLPTR);

    virtual ~PlayListMetaMgrInternal();

    QList<PlayListMeta> metaList() const;

    bool addMeta(const PlayListMeta &meta);

    void tryAdd(const PlayListMeta &meta);

    void deleteMeta(const PlayListMeta &meta);

    void updateMeta(const PlayListMeta &old,  const PlayListMeta &newMeta);

    PlayListMeta create();

    void saveToDatabase();

    static QString formatTimeStamp(const PlayListMeta &meta, QStringView format);

signals:
   void metaDataChanged(PlayListMetaMgr::UpdateMetaRet ret, const PlayListMeta &old, const PlayListMeta &newMeta);

    void metaAdded(const PlayListMeta &meta);

    void metaDeleted(const PlayListMeta &meta);

private:
    void readDatabase();

    /*!
     * \brief nameConflict compare name and namesuffix of two PlayListMeta
     * \param old
     * \param newMeta
     * \return true if conflict
     */
    inline bool nameConflict(const PlayListMeta &old, const PlayListMeta &newMeta)
    {
        return (old.getFileName() == newMeta.getFileName())
                && (old.getFileSuffix() == newMeta.getFileSuffix());
    }

private:
    int                                 m_nameSuffix;
    QSharedPointer<PPSettingsInternal>  m_settings;
    QString                             m_dbPath;
    QList<PlayListMeta>                 m_metaList;
};

} // namespace PhoenixPlayer

#endif // PLAYLISTMETAMGRINTERNAL_H
