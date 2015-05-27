
#include "AddonMgr.h"

#include <QDebug>
#include <QDir>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>

#include "Util.h"

namespace PhoenixPlayer {

const static char *MANIFEST = "manifest.json";
const static char *INDEX_FILE = "main.qml";

const static char *KEY_NAME =  "name";
const static char *KEY_ICON = "icon";
const static char *KEY_VERSION = "version";
const static char *KEY_MAINTAINER = "maintainer";
const static char *KEY_DESCRIPTION = "description";
AddonMgr::AddonMgr(QObject *parent)
    : QObject(parent)
{
    initList ();
}

AddonMgr::~AddonMgr()
{
    if (!mAddonList.isEmpty ()) {
        qDeleteAll(mAddonList);
        mAddonList.clear ();
    }
}

QList<AddonMgr::Addon *> AddonMgr::getAddonList()
{
    return mAddonList;
}

void AddonMgr::initList()
{
    foreach (QString path, Util::getAddonDirList ()) {
        qDebug()<<Q_FUNC_INFO<<" search in path "<<path;
        QDir dir(path);
        QStringList aList = dir.entryList (QDir::Dirs);
        foreach (QString addonDir, aList) {
            QString manifest = QString("%1/%2/%3").arg (path).arg (addonDir).arg (MANIFEST);
            QFile f(manifest);
            if (!f.exists ())
                continue;
            qDebug()<<Q_FUNC_INFO<<" find manifest "<<manifest;
            if (!f.open (QFile::ReadOnly | QFile::Truncate)) {
                qDebug()<<Q_FUNC_INFO<<" open manifest error";
                continue;
            }
            QTextStream stream(&f);
            QString str = stream.readAll ().trimmed ();
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson (str.toLocal8Bit (), &error);
            if (error.error != QJsonParseError::NoError) {
                qDebug()<<Q_FUNC_INFO<<" parse json error "<<error.errorString ();
                continue;
            }
            QJsonObject o = doc.object ();//jv.toObject ();
            Addon *a = new Addon;
            a->basePath = QString("%1/%2").arg (path).arg (addonDir);
            a->description = o.value (KEY_DESCRIPTION).toString ();
            a->icon = o.value (KEY_ICON).toString ();
            a->indexFile = QString(INDEX_FILE);
            a->maintainer = o.value (KEY_MAINTAINER).toString ();
            a->name = o.value (KEY_NAME).toString (QString("UnKnow Addon"));
            a->version = o.value (KEY_VERSION).toString ();
            a->hash = Util::calculateHash (a->basePath + a->indexFile);
            mAddonList.append (a);
        }
    }
}



} //PhoenixPlayer
