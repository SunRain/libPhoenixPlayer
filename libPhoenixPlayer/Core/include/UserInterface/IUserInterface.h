#ifndef IUSERINTERFACE_H
#define IUSERINTERFACE_H

#include <QObject>
#include <QJsonObject>

#include "libphoenixplayer_global.h"
#include "BasePlugin.h"


namespace PhoenixPlayer {
    namespace UserInterface {

class IUserInterface : public BasePlugin
{
    Q_OBJECT
public:
    explicit IUserInterface(QObject *parent = Q_NULLPTR);
    virtual ~IUserInterface() override;

    virtual bool initialize() = 0;

    virtual void show() = 0;

    virtual void hide() = 0;

    virtual void close() = 0;

    /*!
     * \brief snapshots
     * \return QStringList of snapshot, formatted with qrc type
     */
    virtual QStringList snapshots() const = 0;

    static QStringList convertToSnapshots(const QByteArray &extraData);

    static QVariant convertToExtraData(const QStringList &snapshots);

    // BasePlugin interface
public:
    virtual PluginProperty property() const Q_DECL_OVERRIDE
    {
        return PluginProperty();
    }
    virtual PluginType type() const Q_DECL_OVERRIDE
    {
        return BasePlugin::PluginUserInterface;
    }
};

} //namespace UserInterface
} //namespace PhoenixPlayer


#endif // IUSERINTERFACE_H
