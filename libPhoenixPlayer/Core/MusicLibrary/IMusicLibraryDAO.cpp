#include "MusicLibrary/IMusicLibraryDAO.h"

#include <QEvent>
#include <QApplication>

namespace PhoenixPlayer {
namespace MusicLibrary {

#define EVENT_MUSICLIB_DAO   (QEvent::Type(QEvent::User + 1000))

class MusicLibDaoEvent : public QEvent
{
public:
    enum OperationType
    {
        OP_UpdataMeta = 0x0,
        OP_DeleteMeta,
        OP_InsertMeta,
        OP_Undefined
    };
    explicit MusicLibDaoEvent(OperationType type, const AudioMetaObject &obj)
        : QEvent(EVENT_MUSICLIB_DAO),
          m_type(type),
          m_obj(obj)
    {

    }
    virtual ~MusicLibDaoEvent()
    {

    }
    inline OperationType type() const
    {
        return m_type;
    }
    inline AudioMetaObject audioObject() const
    {
        return  m_obj;
    }
private:
    OperationType   m_type = OP_Undefined;
    AudioMetaObject m_obj;

};

IMusicLibraryDAO::IMusicLibraryDAO(QObject *parent)
    : BasePlugin(parent)
{

}

IMusicLibraryDAO::~IMusicLibraryDAO()
{

}

bool IMusicLibraryDAO::insertMetaData(const AudioMetaObject &obj, bool skipDuplicates)
{
    if (doInsertMetaData(obj, skipDuplicates)) {
        qApp->postEvent(this, new MusicLibDaoEvent(MusicLibDaoEvent::OP_InsertMeta, obj));
        return true;
    }
    return false;
}

bool IMusicLibraryDAO::updateMetaData(const AudioMetaObject &obj, bool skipEmptyValue)
{
    if (doUpdateMetaData(obj, skipEmptyValue)) {
        qApp->postEvent(this, new MusicLibDaoEvent(MusicLibDaoEvent::OP_UpdataMeta, obj));
        return true;
    }
    return false;
}

bool IMusicLibraryDAO::deleteMetaData(const AudioMetaObject &obj)
{
    if (doDeleteByHash(obj.hash())) {
        qDebug()<<" --------------- delete  ok";
        qApp->postEvent(this, new MusicLibDaoEvent(MusicLibDaoEvent::OP_DeleteMeta, obj));
        return true;
    }
    qDebug()<<" --------------- delete  failure";
    return false;
}

bool IMusicLibraryDAO::deleteByHash(const QString &hash)
{
    AudioMetaObject obj = trackFromHash(hash);
    if (obj.isHashEmpty()) {
        qWarning()<<"The deleted hash "<<hash<<" not found in database !!";
        return false;
    }
    return deleteMetaData(obj);
}

bool IMusicLibraryDAO::event(QEvent *ev)
{
    if (ev->type() == EVENT_MUSICLIB_DAO) {
        const AudioMetaObject obj = ((MusicLibDaoEvent *)ev)->audioObject();
        const MusicLibDaoEvent::OperationType type = ((MusicLibDaoEvent*)ev)->type();

        qDebug()<<" ---------------- event is "<<ev;

        if (type == MusicLibDaoEvent::OP_DeleteMeta) {
            Q_EMIT(metaDataDeleted(obj.hash()));
        } else if (type == MusicLibDaoEvent::OP_UpdataMeta) {
            Q_EMIT(metaDataChanged(obj.hash()));
        } else if (type == MusicLibDaoEvent::OP_InsertMeta) {
            Q_EMIT(metaDataInserted(obj.hash()));
        }
        return true;
    }
    return BasePlugin::event(ev);
}



} //IMUSICLIBRARYDAO_H
} //PhoenixPlayer
