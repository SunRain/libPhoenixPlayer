#ifndef USERINTERFACEMGR_H
#define USERINTERFACEMGR_H

#include <QObject>
#include <QSharedPointer>

#include "libphoenixplayer_global.h"
#include "PluginMgr.h"

namespace PhoenixPlayer {

    class UserInterfaceMgrInternal;

    namespace UserInterface {

        class IUserInterface;

/*!
 * \brief The UserInterfaceMgr class
 * Internal Singleton class
 */
class UserInterfaceMgr : public QObject
{
    Q_OBJECT
public:
    explicit UserInterfaceMgr(QObject *parent = Q_NULLPTR);
    virtual ~UserInterfaceMgr();

    /*!
     * \brief usedInterface
     * \return Current used user interface or default fallback user interface.
     * Return Q_NULLPTR if no valid user interface plugin found.
     */
    IUserInterface *usedInterface() const;

    void setUsedInterface(const PluginMetaData &data);

    void setUsedInterface(const QString &libraryFile);

    PluginMetaData usedPluginMetaData() const;

    /*!
     * \brief reload Force to reload internal plugin list
     */
    void reload();

private:
    QSharedPointer<UserInterfaceMgrInternal> m_internal;
};



} // namespace UserInterface
} // namespace PhoenixPlayer
#endif // USERINTERFACEMGR_H
