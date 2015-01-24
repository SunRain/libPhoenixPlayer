#ifndef IMUSICTAGPARSER
#define IMUSICTAGPARSER

#include <QObject>

namespace PhoenixPlayer {
class SongMetaData;
namespace MusicLibrary {

class IMusicTagParser : public QObject
{
    Q_OBJECT
public:
    explicit IMusicTagParser(QObject *parent = 0) : QObject(parent) {}
    virtual ~IMusicTagParser() {}

    ///
    /// \brief parserTag 解析文件tag
    /// \param targetMetaDate
    /// \return 成功返回true
    ///
    virtual bool parserTag(SongMetaData *targetMetaDate) = 0;
};

} //MusicLibrary
} //PhoenixPlayer

Q_DECLARE_INTERFACE(PhoenixPlayer::MusicLibrary::IMusicTagParser, "PhoenixPlayer.MusicLibrary.IMusicTagParser/1.0")

#endif // IMUSICTAGPARSER

