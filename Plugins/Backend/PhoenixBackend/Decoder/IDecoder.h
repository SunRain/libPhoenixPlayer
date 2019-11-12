#ifndef IDECODER_H
#define IDECODER_H

#include <QObject>
#include <QtGlobal>

#include "LibPhoenixPlayerMain.h"

#include "AudioParameters.h"

namespace PhoenixPlayer {

    class MediaResource;

    namespace PlayBackend {

        namespace PhoenixBackend {

            namespace Decoder {

class IDecoder : public QObject
{
    Q_OBJECT
public:
    IDecoder(QObject *parent = Q_NULLPTR);
    virtual ~IDecoder();

    virtual bool initialize(MediaResource *res) = 0;

    ///
    /// \brief durationInSeconds
    /// \return duration in seconds, <=0 if not find duration
    ///
    virtual qint64 durationInSeconds() = 0;

    virtual void setPositionMS(qreal millisecond = 0) = 0;

    /*!
     * Returns current bitrate (in kbps).
     * Subclass should reimplement this function.
     */
    virtual int bitrate() = 0;

    /*
     * Reads up to maxSize bytes of decoded audio to data
     * Returns the number of bytes read, or -1 if an error occurred.
     * Subclass should reimplement this function.
     */
    virtual qint64 read(unsigned char *data, qint64 maxSize) = 0;

    ///
    /// \brief audioParameters
    /// \return detected audio parameters
    ///
    virtual AudioParameters audioParameters() const = 0;
};
} //Decoder
} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

Q_DECLARE_INTERFACE(PhoenixPlayer::PlayBackend::PhoenixBackend::Decoder::IDecoder,
                    "PhoenixPlayer.PhoenixBackend.Decoder.BaseDecoderInterface/1.0")
#endif // IDECODER_H