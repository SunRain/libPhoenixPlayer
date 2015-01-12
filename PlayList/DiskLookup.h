#ifndef DISKLOOKUP_H
#define DISKLOOKUP_H

#include <QObject>

namespace PhoenixPlayer {
namespace PlayList {
class DiskLookup : public QObject
{
    Q_OBJECT
public:
    explicit DiskLookup(QObject *parent = 0);
    static DiskLookup *getInstance();
    virtual ~DiskLookup();

    void lookup();
    void setDir(const QString &dirName);

protected:
    void scanDir(const QString &dir);
signals:
    void fileFound(QString path, QString file);

private:


public slots:
};

} //PlayList
} //PhoenixPlayer
#endif // DISKLOOKUP_H
