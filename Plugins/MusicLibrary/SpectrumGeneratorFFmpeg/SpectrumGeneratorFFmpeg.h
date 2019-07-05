#ifndef SPECTRUMGENERATORFFMPEG_H
#define SPECTRUMGENERATORFFMPEG_H

#include "MusicLibrary/ISpectrumGenerator.h"

namespace PhoenixPlayer {
namespace MusicLibrary {
namespace SpectrumGeneratorFFmpeg {

class SpekSpectrogram;
class SpectrumGeneratorFFmpeg : public ISpectrumGenerator
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.SpectrumGenerator.SpectrumGeneratorFFmpeg" FILE "SpectrumGeneratorFFmpeg.json")
    Q_INTERFACES(PhoenixPlayer::MusicLibrary::ISpectrumGenerator)

public:
    explicit SpectrumGeneratorFFmpeg(QObject *parent = nullptr);
    virtual ~SpectrumGeneratorFFmpeg() override;

    // ISpectrumGenerator interface
public:
    QList<QList<qreal> > generate(const AudioMetaObject &obj) const Q_DECL_OVERRIDE;

private:
    SpekSpectrogram                 *m_spek;
};

} //SpectrumGeneratorFFmpeg
} //MusicLibrary
} //PhoenixPlayer
#endif // SPECTRUMGENERATORFFMPEG_H
