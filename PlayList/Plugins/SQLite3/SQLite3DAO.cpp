#include "SQLite3DAO.h"

namespace PhoenixPlayer {
namespace PlayList {
namespace SQLite3 {


SQLite3DAO::SQLite3DAO(QObject *parent)
    :IPlayListDAO(parent)
{

}

SQLite3DAO::~SQLite3DAO()
{

}

QString SQLite3DAO::getPluginName()
{
    return DAO_NAME;
}

QString SQLite3DAO::getPluginVersion()
{
    return VERSION;
}

bool SQLite3DAO::initDataBase()
{
    return true;
}

bool SQLite3DAO::openDataBase()
{
    return true;
}

bool SQLite3DAO::beginTransaction()
{
    return true;
}

bool SQLite3DAO::commitTransaction()
{
    return true;
}

bool SQLite3DAO::insertMetaData(SongMetaData *metaData)
{
    return true;
}

bool SQLite3DAO::updateMetaData(SongMetaData *metaData)
{
    return true;
}

bool SQLite3DAO::deleteMetaData(SongMetaData *metaData)
{
    return true;
}

} //SQLite3
} //PlayList
} //PhoenixPlayer
