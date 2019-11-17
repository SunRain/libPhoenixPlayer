#ifndef USERINTERFACEFAKE_H
#define USERINTERFACEFAKE_H

#include "UserInterface/IUserInterface.h"

namespace PhoenixPlayer {

    namespace UserInterface {

class LIBPHOENIXPLAYER_EXPORT UserInterfaceFake : public IUserInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.UserInterface.FakeUserInterface" FILE "FakeUserInterface.json")
    Q_INTERFACES(PhoenixPlayer::BasePlugin)
public:
    explicit UserInterfaceFake(QObject *parent = nullptr);
    virtual ~UserInterfaceFake() override;

    // BasePlugin interface
public:
    PluginProperty property() const Q_DECL_OVERRIDE;

    // IUserInterface interface
public:
    bool initialize() Q_DECL_OVERRIDE;
    void show() Q_DECL_OVERRIDE;
    void hide() Q_DECL_OVERRIDE;
    void close() Q_DECL_OVERRIDE;
    QStringList snapshots() const Q_DECL_OVERRIDE;

    // BasePlugin interface
public:
    QWidget *settingsUI() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }
    QWidget *aboutUI() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }
};
} //namespace UserInterface
} //namespace PhoenixPlayer
#endif // USERINTERFACEFAKE_H
