#include "UserInterfaceFake.h"

#include "MainWindow.h"
namespace PhoenixPlayer {
namespace UserInterface {

UserInterfaceFake::UserInterfaceFake(QObject *parent)
    : IUserInterface(parent)
{

}

UserInterfaceFake::~UserInterfaceFake()
{

}

PluginProperty UserInterfaceFake::property() const
{
    return PluginProperty("UserInterfaceFake",
                          "1.0",
                          "Fake UI",
                          true,
                          true);
}

bool UserInterfaceFake::initialize()
{
    return true;
}

void UserInterfaceFake::show()
{
    MainWindow *w = new MainWindow;
    w->show();
}

void UserInterfaceFake::hide()
{

}

void UserInterfaceFake::close()
{

}

QStringList UserInterfaceFake::snapshots() const
{
    return QStringList();
}

} //namespace UserInterface
} //namespace PhoenixPlayer
