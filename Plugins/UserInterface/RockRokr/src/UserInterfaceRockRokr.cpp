#include "UserInterfaceRockRokr.h"

#include "MusicLibrary/MusicLibraryManager.h"
#include "MainWindow.h"

namespace PhoenixPlayer {
namespace UserInterface {
namespace RockRokr {

UserInterfaceRockRokr::UserInterfaceRockRokr(QObject *parent)
    : IUserInterface(parent)
{

}

UserInterfaceRockRokr::~UserInterfaceRockRokr()
{
    if (m_mainWindow) {
        m_mainWindow->deleteLater();
        m_mainWindow = Q_NULLPTR;
    }
}

PluginProperty UserInterfaceRockRokr::property() const
{
  return PluginProperty("RockRokrUI",
                        "1.0",
                        "RockRokr UI use deepin DTK widget",
                        true,
                        true);
}

bool UserInterfaceRockRokr::initialize()
{
    if (!m_mainWindow) {
        m_mainWindow = new MainWindow;
    }
    if (!m_libraryMgr) {
        m_libraryMgr = new MusicLibrary::MusicLibraryManager(this);
    }

    return true;
}

void UserInterfaceRockRokr::show()
{
#if 1
    if (m_libraryMgr->isEmpty()) {
        m_mainWindow->showImportView();
    } else {
        m_mainWindow->showRockRokrView();
    }
#else
    m_mainWindow->showImportView();
    //m_topStack->setCurrentWidget(m_mainStack);
    //m_mainStack->setCurrentWidget(m_rkView);
#endif

    m_mainWindow->show();
}

void UserInterfaceRockRokr::hide()
{

}

void UserInterfaceRockRokr::close()
{

}

QStringList UserInterfaceRockRokr::snapshots() const
{
    return QStringList();
}







} //namespace RockRokr
} //namespace UserInterface
} //namespace PhoenixPlayer
