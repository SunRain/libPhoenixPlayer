#include "LibPhoenixPlayerMain.h"

namespace PhoenixPlayer {
using namespace MusicLibrary;
using namespace MetadataLookup;

LibPhoenixPlayer::LibPhoenixPlayer(QObject *parent)
    : QObject(parent)
    , m_initiated(false)
    , m_settings(nullptr)
    , m_libraryMgr(nullptr)
    , m_volumeCtrl(nullptr)
    , m_playerCore(nullptr)
    , m_lookupMgr(nullptr)
{

}

LibPhoenixPlayer::~LibPhoenixPlayer()
{

}

void LibPhoenixPlayer::initiate()
{
    if (m_initiated)
        return;
}

Settings *LibPhoenixPlayer::settings()
{
    return m_settings;
}

MusicLibraryManager *LibPhoenixPlayer::libraryMgr()
{
    return m_libraryMgr;
}

VolumeControl *LibPhoenixPlayer::volumeCtrl()
{
    return m_volumeCtrl;
}

PlayerCore *LibPhoenixPlayer::playerCore()
{
    return m_playerCore;
}

MetadataLookupMgrWrapper *LibPhoenixPlayer::metaLookupMgr()
{
    return m_lookupMgr;
}





}
