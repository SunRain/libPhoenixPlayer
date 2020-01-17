#include "PlayerCore/PlayListMetaMgr.h"

#include "private/SingletonObjectFactory.h"
#include "private/PlayListMetaMgrInternal.h"

namespace PhoenixPlayer {

PlayListMetaMgr::PlayListMetaMgr(QObject *parent)
    : QObject(parent)
{
    m_internal = SingletonObjectFactory::instance()->plsMgrInternal();

    connect(m_internal.data(), &PlayListMetaMgrInternal::metaDataChanged,
            this, &PlayListMetaMgr::metaDataChanged, Qt::QueuedConnection);

    connect(m_internal.data(), &PlayListMetaMgrInternal::metaAdded,
            this, &PlayListMetaMgr::metaAdded, Qt::QueuedConnection);

    connect(m_internal.data(), &PlayListMetaMgrInternal::metaDeleted,
            this, &PlayListMetaMgr::metaDeleted, Qt::QueuedConnection);

}

PlayListMetaMgr::~PlayListMetaMgr()
{
    m_internal->saveToDatabase();
    m_internal->disconnect(this);
}

QList<PlayListMeta> PlayListMetaMgr::metaList() const
{
    return m_internal->metaList();
}

bool PlayListMetaMgr::addMeta(const PlayListMeta &meta)
{
    return m_internal->addMeta(meta);
}

void PlayListMetaMgr::tryAdd(const PlayListMeta &meta)
{
    m_internal->tryAdd(meta);
}

void PlayListMetaMgr::deleteMeta(const PlayListMeta &meta)
{
    m_internal->deleteMeta(meta);
}

void PlayListMetaMgr::updateMeta(const PlayListMeta &old, const PlayListMeta &newMeta, bool ignoreNameConflict)
{
    m_internal->updateMeta(old, newMeta, ignoreNameConflict);
}

PlayListMeta PlayListMetaMgr::create()
{
    return m_internal.data()->create();
}

void PlayListMetaMgr::saveToDatabase()
{
    m_internal->saveToDatabase();
}

QString PlayListMetaMgr::formatTimeStamp(const PlayListMeta &meta, QStringView format)
{
    return PlayListMetaMgrInternal::formatTimeStamp(meta, format);
}



} // namespace PhoenixPlayer
