#ifndef SONGMETADATA_H
#define SONGMETADATA_H

#include <QObject>
#include <QHash>
#include <QVariant>
#include <QDate>
#include "Common.h"

class QStringList;
class QDate;
namespace PhoenixPlayer {
class SongMetaData : public QObject
{
    Q_OBJECT
public:
    explicit SongMetaData(QObject *parent = 0);
    virtual ~SongMetaData();

    void setMeta(Common::SongMetaTags tagType, const QVariant &value);
    QVariant getMeta(Common::SongMetaTags tagType) const;
    QString toString();
    bool operator == (const SongMetaData &other) const;
private:
    QHash<Common::SongMetaTags, QVariant> m_metaHash;
};

} //PhoenixPlayer
#endif // SONGMETADATA_H
