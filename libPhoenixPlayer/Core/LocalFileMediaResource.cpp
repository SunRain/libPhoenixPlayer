#include "LocalFileMediaResource.h"

#include <QFile>

namespace PhoenixPlayer {

LocalFileMediaResource::LocalFileMediaResource(const QString &uri, QObject *parent)
    : MediaResource(uri, parent)
    , m_file(nullptr)
{
//    m_file = new QFile(uri, this);
}

LocalFileMediaResource::~LocalFileMediaResource()
{
    if (m_file->isOpen ())
        m_file->close ();
    m_file->deleteLater ();
    m_file = nullptr;
}

bool LocalFileMediaResource::initialize()
{
    m_file = new QFile(getUri (), this);
    return true;
}

QIODevice *LocalFileMediaResource::device()
{
    return m_file;
}

bool LocalFileMediaResource::pending() const
{
    return false;
}

bool LocalFileMediaResource::ready() const
{
    return m_file->isOpen ();
}

PPCommon::MediaType LocalFileMediaResource::type() const
{
    return PPCommon::MediaTypeLocalFile;
}



} //PhoenixPlayer
