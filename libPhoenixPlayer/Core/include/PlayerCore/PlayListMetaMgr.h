#ifndef PLAYLISTMETAMGR_H
#define PLAYLISTMETAMGR_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QSharedPointer>

#include "libphoenixplayer_global.h"
#include "PlayerCore/PlayListObject.h"
#include "PlayerCore/PlayListMeta.h"

namespace PhoenixPlayer {

class PlayListMetaMgrInternal;

/*!
 * \brief The PlayListMetaMgr class
 * Internal Singleton class
 */
class LIBPHOENIXPLAYER_EXPORT PlayListMetaMgr : public QObject
{
    Q_OBJECT

public:
    enum UpdateMetaRet {
        OK = 0x0,
        NameConflict
    };
    Q_ENUM(UpdateMetaRet)

    explicit PlayListMetaMgr(QObject *parent = Q_NULLPTR);

    virtual ~PlayListMetaMgr();

    QList<PlayListMeta> metaList() const;

    /*!
     * \brief addMeta
     * \param meta
     * \return True if list not contains this meta(same meta name and suffix)
     * False if list contains this meta(same meta name and suffix)
     */
    bool addMeta(const PlayListMeta &meta);

    /*!
     * \brief tryAdd try to add meta to list
     * If list not contains this meta, signal addedMeta will be emmited
     * If list contains this meta(same meta name and suffix), nothing happed
     * \param meta
     */
    void tryAdd(const PlayListMeta &meta);

    /*!
     * \brief deleteMeta
     * \param meta
     */
    void deleteMeta(const PlayListMeta &meta);

    /*!
     * \brief updateMeta
     * \param old
     * \param newMeta
     * \param ignoreNameConflict set to True if only update meta content(eg. tag, annotation)
     */
    void updateMeta(const PlayListMeta &old,  const PlayListMeta &newMeta, bool ignoreNameConflict = false);

    PlayListMeta create();

    void saveToDatabase();

    static QString formatTimeStamp(const PlayListMeta &meta, QStringView format);

signals:
    void metaDataChanged(UpdateMetaRet ret, const PlayListMeta &old, const PlayListMeta &newMeta);

    void metaAdded(const PlayListMeta &meta);

    void metaDeleted(const PlayListMeta &meta);

private:
    QSharedPointer<PlayListMetaMgrInternal> m_internal;

};

} //PhoenixPlayer

#endif // PLAYLISTMETAMGR_H
