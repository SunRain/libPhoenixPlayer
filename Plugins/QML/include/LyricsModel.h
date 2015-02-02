#ifndef LYRICSMODEL_H
#define LYRICSMODEL_H

#include <QAbstractListModel>
#include "LyricsParser.h"

namespace PhoenixPlayer {
namespace QmlPlugin {

class LyricsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

public:
    LyricsModel(QObject *parent = 0);
    ~LyricsModel();

    Q_INVOKABLE void setLyricsStr(const QString &lyrics);
    Q_INVOKABLE void findIndex(quint64 tick);

    int currentIndex();
    void setCurrentIndex(int index);

    enum LyricsRoles {
        TimeRole = Qt::UserRole + 1,
        TextRole
    };

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

signals:
    void currentIndexChanged();

private:
    MetadataLookup::QLyricsList mLyricsList;
    int mCurrentIndex;


};

} //QmlPlugin
} //PhoenixPlayer
#endif // LYRICSMODEL_H
