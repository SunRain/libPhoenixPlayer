#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <QObject>

#include "PluginMgr.h"

class QApplication;
namespace PhoenixPlayer {

    namespace UserInterface {
        class IUserInterface;
    }

class ApplicationManager
{
public:
    explicit ApplicationManager(int &argc, char **argv[]);
    virtual ~ApplicationManager();

    void initialization();

    inline QApplication *application() const
    {
        return m_app;
    }

public Q_SLOTS:
    void exec();

private:
    QApplication                    *m_app = Q_NULLPTR;
    UserInterface::IUserInterface   *m_userInterface = Q_NULLPTR;

    PluginMetaData m_usedUIMetaData;
};

} // namespace PhoenixPlayer


#endif // APPLICATIONMANAGER_H
