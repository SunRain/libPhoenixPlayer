
#include "PathListModel.h"


namespace PhoenixPlayer {

namespace QmlPlugin {

PathListModel::PathListModel(QAbstractListModel *parent)
    :QAbstractListModel(parent)
{
    mBasePath = QString();
    mCurrentPath = QString();
    mLimitDepath = true;
}

PathListModel::~PathListModel()
{
    if (!mPathItemList.isEmpty ())
        mPathItemList.clear ();
}

void PathListModel::cdUp()
{
    if (mLimitDepath && mCurrentPath == mBasePath)
        return;
    if (!mDir.cdUp ())
        return;
    mCurrentPath = mDir.absolutePath ();
    emit currentPathChanged ();
    clear ();
    if (mDir.exists ())
        mPathItemList = mDir.entryList (QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    appendList ();
}

QString PathListModel::getCurrentPath()
{
    return mCurrentPath;
}

void PathListModel::jumpTo(const QString &newPath, bool isAbstractPath)
{
    if (mCurrentPath == newPath)
        return;

    clear ();
    if (isAbstractPath) {
        mCurrentPath = newPath;
    } else {
        mCurrentPath = QString("%1/%2").arg (mCurrentPath).arg (newPath);
    }
    emit currentPathChanged ();
    mDir.setPath (mCurrentPath);
    if (mDir.exists ())
        mPathItemList = mDir.entryList (QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    appendList ();
}

QString PathListModel::basePath()
{
    return mBasePath;
}

bool PathListModel::limitDepth()
{
    return mLimitDepath;
}

void PathListModel::setLimitDepth(bool limit)
{
    mLimitDepath = limit;
    emit limitDepthChanged ();
}

void PathListModel::appendList()
{
    if (mPathItemList.isEmpty ())
        return;

    for (int i=0; i<mPathItemList.size (); ++i) {
        beginInsertRows (QModelIndex(), i, i);
        endInsertRows ();
    }
}

void PathListModel::clear()
{
    beginResetModel ();
    mPathItemList.clear ();
    endResetModel ();
}

void PathListModel::setBasePath(const QString &path)
{
    mBasePath = path;
    mCurrentPath = mBasePath;
    emit basePathChanged ();
    emit currentPathChanged ();

    clear ();
    mDir.setPath (mBasePath);
    if (mDir.exists ())
        mPathItemList = mDir.entryList (QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    appendList ();
}

QHash<int, QByteArray> PathListModel::roleNames() const
{
    QHash<int, QByteArray> role;
    role.insert (ModelRoles::RolePath, "pathName");
    return role;
}

QVariant PathListModel::data(const QModelIndex &index, int role) const
{
    if (index.row () < 0 || index.row () >= mPathItemList.size ())
        return QVariant();
    switch (role) {
    case ModelRoles::RolePath:
        return mPathItemList.at (index.row ());
    default:
        return QVariant();
    }
}

int PathListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mPathItemList.size ();
}
} //QmlPlugin
} //PhoenixPlayer
