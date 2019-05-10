#include "PlayerCore/PlayListObjectMgr.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

#include "PPSettings.h"
#include "M3uPlayListFormat.h"

PhoenixPlayer::PlayListObjectMgr::PlayListObjectMgr(PPSettings *set, QObject *parent)
    : QObject(parent),
      m_settings(set)
{
    m_playListDir = m_settings->playListDir();

    connect(m_settings, &PPSettings::playListDirChanged,
             [&](QString arg) {
        m_playListDir = arg;
        queryPlayLists();
    });
    queryPlayLists();

}

PhoenixPlayer::PlayListObjectMgr::~PlayListObjectMgr()
{

}

void PhoenixPlayer::PlayListObjectMgr::refreshExistPlayLists()
{
    queryPlayLists();
}

QStringList PhoenixPlayer::PlayListObjectMgr::existPlayLists() const
{
    return m_existPlayLists;
}

void PhoenixPlayer::PlayListObjectMgr::setExistPlayLists(const QStringList &existPlayLists)
{
    if (m_existPlayLists == existPlayLists) {
        return;
    }
    m_existPlayLists = existPlayLists;
    emit existPlayListsChanged(existPlayLists);
}

void PhoenixPlayer::PlayListObjectMgr::queryPlayLists()
{
    QDir dir(m_playListDir);
    if (!dir.exists ()) {
        qWarning()<<Q_FUNC_INFO<<"No playList dir exist!!";
        return;
    }
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    QFileInfoList list = dir.entryInfoList ();
    QStringList plst;
    //TODO soupport more playlist format
    M3uPlayListFormat f;
    foreach (QFileInfo info, list) {
        QString s = info.completeSuffix ().toLower ();
        if (s.contains (f.extension ().toLower ())
                || f.extension ().toLower ().contains (s)) {
            plst.append (info.baseName ());
        }
    }
    setExistPlayLists(plst);
}
