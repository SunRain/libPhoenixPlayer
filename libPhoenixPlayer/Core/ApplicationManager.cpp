#include "ApplicationManager.h"

#include <QApplication>

#ifdef PLATFORM_DEEPIN
    #include <DApplication>
    #include <DThemeManager>
    #include <DWidgetUtil>
    DWIDGET_USE_NAMESPACE
#endif

#include "PluginMgr.h"
#include "LogManager.h"
#include "UserInterface/IUserInterface.h"
#include "UserInterface/UserInterfaceMgr.h"

namespace PhoenixPlayer {

ApplicationManager::ApplicationManager(int &argc, char **argv[])
{
#ifdef PLATFORM_DEEPIN
    DApplication::loadDXcbPlugin();
#endif
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);

#ifdef PLATFORM_DEEPIN
        m_app = new DApplication(argc, *argv);
        qobject_cast<DApplication*>(m_app)->loadTranslator();
        qobject_cast<DApplication*>(m_app)->setTheme("light");
    #ifdef Q_OS_UNIX
        qobject_cast<DApplication*>(m_app)->setOOMScoreAdj(500);
    #endif
#else
    m_app = new QApplication(argc, *argv);
#endif

    for (int i = 1; i < argc; ++i) {
        if (!qstrcmp(*argv[i], "-no-gui")) {
            //TODO no gui mode
        }
    }
}

ApplicationManager::~ApplicationManager()
{
    if (PluginMetaData::isValid(m_usedUIMetaData) && m_userInterface) {
        m_userInterface->close();
        PluginMgr::unload(m_usedUIMetaData);
        m_userInterface = Q_NULLPTR;
    }

    if (m_app) {
        m_app->deleteLater();
    }
}

void ApplicationManager::initialization()
{
    DLogManager::registerConsoleAppender();


    if (!m_userInterface) {
        UserInterface::UserInterfaceMgr mgr;
        m_userInterface = mgr.usedInterface();
        if (m_userInterface) {
            m_usedUIMetaData = mgr.usedPluginMetaData();
        }
    }
    if (m_userInterface) {
        m_userInterface->initialize();
    }
}

void ApplicationManager::exec()
{
    if (m_userInterface) {
        m_userInterface->show();
    }

    m_app->exec();
}




} // namespace PhoenixPlayer
