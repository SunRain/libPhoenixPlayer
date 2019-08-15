#include "MainWindow.h"

#include <QDebug>

#include "PhoenixPlayBackend.h"
#include "MediaResource.h"

using namespace PhoenixPlayer;
using namespace PhoenixPlayer::PlayBackend::PhoenixBackend;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    PhoenixPlayBackend *bk = new PhoenixPlayBackend(this);
    bk->initialize();

    MediaResource *res = MediaResource::create("/home/wangguojian/音乐/faraway.mp3", this);

    bk->changeMedia(res, 0, false);

    bk->play();


}

MainWindow::~MainWindow()
{

}
