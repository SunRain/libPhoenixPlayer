#include "UserInterface/IUserInterface.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>

#include "Logger.h"

namespace PhoenixPlayer {
namespace UserInterface {

const static char *KEY_SNAPSHOT = "KEY_SNAPSHOT";

IUserInterface::IUserInterface(QObject *parent)
    : BasePlugin(parent)
{

}

IUserInterface::~IUserInterface()
{

}

QStringList IUserInterface::convertToSnapshots(const QByteArray &extraData)
{
    if (extraData.isEmpty()) {
        return QStringList();
    }
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(extraData, &error);
    if (error.error != QJsonParseError::NoError) {
        LOG_WARNING()<<"Parse extra data failure "<<error.errorString();
        return QStringList();
    }
    QJsonObject object = doc.object();
    if (object.isEmpty()) {
        LOG_WARNING()<<"Json object is empty";
        return QStringList();
    }
    QJsonArray array = object.value(KEY_SNAPSHOT).toArray();
    if (array.isEmpty()) {
        return QStringList();
    }
    QStringList list;
    foreach (const auto &it, array) {
        list.append(it.toString().toUtf8());
    }
    return list;
}

QByteArray IUserInterface::convertToExtraData(const QStringList &snapshots)
{
    if (snapshots.isEmpty()) {
        return QByteArray();
    }
    QJsonObject object;
    QJsonArray array;
    foreach (const auto &it, snapshots) {
        array.append(it);
    }
    object.insert(KEY_SNAPSHOT, array);
    QJsonDocument doc(object);
    return doc.toJson(QJsonDocument::JsonFormat::Compact);
}


} //namespace UserInterface
} //namespace PhoenixPlayer
