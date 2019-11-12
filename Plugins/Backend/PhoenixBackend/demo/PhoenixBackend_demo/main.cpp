#include "MainWindow.h"
#include <QApplication>
#include <QDebug>

#include "libphoenixplayer_global.h"
//#include "LibPhoenixPlayerMain.h"
//#include "PluginLoader.h"
#include "PluginMgr.h"

using namespace PhoenixPlayer;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    PhoenixPlayer::LibPhoenixPlayer_Init();

    qDebug()<<Q_FUNC_INFO<<" applicationDirPath "<<QCoreApplication::applicationDirPath();

//    phoenixPlayerLib->pluginLoader()->addPluginPath(QCoreApplication::applicationDirPath());

    PhoenixPlayer::PluginMgr mgr;
    mgr.addPluginPath(QCoreApplication::applicationDirPath()+"/plugins");

    MainWindow w;
    w.show();

    return a.exec();
}
