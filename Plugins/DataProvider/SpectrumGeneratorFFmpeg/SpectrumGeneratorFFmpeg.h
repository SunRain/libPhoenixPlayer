#ifndef SPECTRUMGENERATORFFMPEG_H
#define SPECTRUMGENERATORFFMPEG_H

#include "DataProvider/ISpectrumGenerator.h"

namespace PhoenixPlayer {
namespace DataProvider {
namespace SpectrumGeneratorFFmpeg {

class SpekAudio;
class SpekFFT;
class SpectrumGeneratorFFmpegInternal;
class SpectrumGeneratorFFmpeg : public ISpectrumGenerator
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.SpectrumGenerator.SpectrumGeneratorFFmpeg" FILE "SpectrumGeneratorFFmpeg.json")
    Q_INTERFACES(PhoenixPlayer::BasePlugin)

public:
    explicit SpectrumGeneratorFFmpeg(QObject *parent = Q_NULLPTR);
    virtual ~SpectrumGeneratorFFmpeg() override;

    // BasePlugin interface
public:
    PluginProperty property() const Q_DECL_OVERRIDE;

    // IDataProvider interface
public:
    bool support(SupportedTypes type) const Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;

    // ISpectrumGenerator interface
public:
    SpectrumDataList generate(const AudioMetaObject &obj) const Q_DECL_OVERRIDE;

private:
    void readSpekAudio() const;

    void processData() const;

    void decorateSpek(int bands, int sample, float *values) const;

private:
    SpekAudio                           *m_spekAudio    = Q_NULLPTR;
    SpekFFT                             *m_spekFFT      = Q_NULLPTR;
    SpectrumGeneratorFFmpegInternal     *m_internal     = Q_NULLPTR;
};

} //SpectrumGeneratorFFmpeg
} //DataProvider
} //PhoenixPlayer
#endif // SPECTRUMGENERATORFFMPEG_H
