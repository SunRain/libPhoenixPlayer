#ifndef IMUSICTAGPARSER
#define IMUSICTAGPARSER

#include <QObject>
#include <QStringList>

namespace PhoenixPlayer {
class SongMetaData;
namespace MusicLibrary {

class IMusicTagParser : public QObject
{
    Q_OBJECT
public:
    explicit IMusicTagParser(QObject *parent = 0);
    virtual ~IMusicTagParser() {
        mList.clear ();
    }

    virtual QString getPluginName() = 0;
    virtual QString getPluginVersion() = 0;
    virtual QString getDescription() = 0;

    ///
    /// \brief parserTag 解析文件tag
    /// \param targetMetaDate
    /// \return 成功返回true
    ///
    virtual bool parserTag(SongMetaData *targetMetaDate) = 0;
protected:
    inline QString indexToGenre(const int &index) {
        return (index > 0 && index < mList.size ())
                ? mList.at (index)
                : "Unknown";
    }

private:
    QStringList mList;
};
} //MusicLibrary
} //PhoenixPlayer

Q_DECLARE_INTERFACE(PhoenixPlayer::MusicLibrary::IMusicTagParser, "PhoenixPlayer.MusicLibrary.IMusicTagParser/1.0")

#endif // IMUSICTAGPARSER

