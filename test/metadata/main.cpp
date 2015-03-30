#include <QCoreApplication>

#include <QDebug>
#include <QStringList>

#include "PluginLoader.h"
#include "SingletonPointer.h"
#include "Settings.h"
#include "MusicLibraryManager.h"
#include "PluginLoader.h"
#include "Player.h"
#include "LyricsModel.h"

#include "SingletonPointer.h"


using namespace PhoenixPlayer;
using namespace PhoenixPlayer::MusicLibrary;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    a.setOrganizationName ("SunRain");
    a.setApplicationName ("PhoenixPlayer");

    PluginLoader *loader = SingletonPointer<PluginLoader>::instance ();
    loader->setNewPlugin (PluginLoader::TypePlayBackend, "GStreamerBackend");
    loader->setParent (&a);

    MusicLibraryManager *manager = SingletonPointer<MusicLibraryManager>::instance ();
    manager->setParent (&a);

    QScopedPointer<Player> musicPlayer(new Player(&a));

    QStringList hashList = manager->querySongMetaElement(Common::E_Hash, QString(), false);

    qDebug()<<"===== hash list "<<hashList.size ();

    foreach (QString hash, hashList) {
        musicPlayer.data ()->lookupAlbumDate (hash);
        musicPlayer.data ()->lookupAlbumDescription (hash);
        musicPlayer.data ()->lookupAlbumImage (hash);
        musicPlayer.data ()->lookupArtistDescription (hash);
        musicPlayer.data ()->lookupArtistImage (hash);
        musicPlayer.data ()->lookupTrackDescription (hash);
    }
    return a.exec();
}
