#ifndef BASEMEDIAOBJECT_H
#define BASEMEDIAOBJECT_H

#include <QObject>
#include "Common.h"
namespace PhoenixPlayerCore {
class BaseMediaObject : public QObject
{
    Q_OBJECT
public:
    explicit BaseMediaObject(QObject *parent = 0);
    virtual ~BaseMediaObject();
    BaseMediaObject *create(const QString &filePath = "", const QString &fileName = "",
                            Common::MediaType mediaType = Common::TypeLocalFile,
                            QObject *parent = 0);

    QString filePath() const;
    void setFilePath(const QString &filePath);

    QString fileName() const;
    void setFileName(const QString &fileName);

    Common::MediaType mediaType() const;
    void setMediaType(const Common::MediaType &mediaType);
private:
    QString mFilePath;
    QString mFileName;
    Common::MediaType mMediaType;

};
}
Q_DECLARE_INTERFACE(PhoenixPlayerCore::BaseMediaObject, "PhoenixPlayerCore.BaseMediaObject/1.0")

#endif // BASEMEDIAOBJECT_H
