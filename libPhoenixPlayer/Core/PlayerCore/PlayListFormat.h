#ifndef PLAYLISTFORMAT_H
#define PLAYLISTFORMAT_H

#include <QObject>
#include <QFile>
#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {
//class AudioMetaObject;
class LIBPHOENIXPLAYER_EXPORT PlayListFormat : public QObject
{
    Q_OBJECT
public:
    explicit PlayListFormat(QObject *parent = 0);
//    virtual bool support(const QString &mimeOrExtension);
    virtual QString extension() const;
    ///
    /// \brief fileList 解析播放列表内容
    /// \param contents 播放列表内容
    /// \return 返回歌曲路径列表
    ///
    virtual QStringList fileList(const QString &contents);

    ///
    /// \brief format 格式化播放列表
    /// \param list 歌曲列表
    /// \return 格式化后的文件内容
    ///
    virtual QString format(const /*QList<AudioMetaObject *>*/AudioMetaList &list);
//    virtual QString encode(const QStringList &fileList);
};
} //PhoenixPlayer
#endif // PLAYLISTFORMAT_H
