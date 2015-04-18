#include <QCoreApplication>

#include <QDebug>
#include <QStringList>

#include "PluginLoader.h"
#include "SingletonPointer.h"
#include "Settings.h"
#include "MusicLibrary/MusicLibraryManager.h"
#include "PluginLoader.h"
#include "Player/Player.h"

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

#if 1
    QStringList hashList = manager->querySongMetaElement(Common::E_Hash, QString(), false);

    qDebug()<<"===== hash list "<<hashList.size ();

    for (int i=0; i<hashList.size (); ++i) {
        if (i == 10) break;
        QString hash = hashList.at (i);
        musicPlayer.data ()->lookupAlbumDate (hash);
        musicPlayer.data ()->lookupAlbumDescription (hash);
        musicPlayer.data ()->lookupAlbumImage (hash);
        musicPlayer.data ()->lookupArtistDescription (hash);
        musicPlayer.data ()->lookupArtistImage (hash);
        musicPlayer.data ()->lookupTrackDescription (hash);
    }
#endif

#if 0
    QStringList metaList = loader->getPluginNames (PluginLoader::TypeMetadataLookup);

    for (int i=0; i<20; ++i) {
        qDebug()<<"======================== \n\n";
        if (i/2 == 0)
            loader->setNewPlugin (PluginLoader::TypeMetadataLookup, metaList[0]);
        else
            loader->setNewPlugin (PluginLoader::TypeMetadataLookup, metaList[1]);
    }
#endif
    return a.exec();
}
