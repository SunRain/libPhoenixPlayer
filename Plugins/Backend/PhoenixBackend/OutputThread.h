/***************************************************************************
 *   Copyright (C) 2012-2019 by Ilya Kotov                                 *
 *   forkotov02@ya.ru                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef OUTPUTTHREAD_H
#define OUTPUTTHREAD_H

#include <QThread>
#include <QMutex>
#include <atomic>

#include "AudioParameters.h"
#include "ChannelMap.h"

#include "PhoenixBackend_global.h"

namespace PhoenixPlayer {
    class PluginLoader;
    class PluginHost;
    class PPSettings;
    class MediaResource;
    class EqualizerMgr;

    namespace PlayBackend {
    class BaseMediaObject;
    class BaseVisual;

        namespace PhoenixBackend {
            class StateHandler;
            class OutputThread;
            class ReplayGain;
            class AudioConverter;
            class Dithering;
            class Buffer;
            class ChannelConverter;
            class Recycler;

            namespace OutPut {
                class IOutPut;
            }

/** @internal
    @brief Output thread.
    @author Ilya Kotov <forkotov02@ya.ru>
*/
class OutputThread : public QThread
{
    Q_OBJECT
public:
    explicit OutputThread(StateHandler *handle, Recycler *recycler, QObject *parent = Q_NULLPTR);

    virtual ~OutputThread() override;
    /*!
     * Prepares object for usage and setups required audio parameters.
     * @param freq Sample rate.
     * @param map Map of channels.
     * @return initialization result (\b true - success, \b false - failure)
     */
    bool initialization(quint32 freq, ChannelMap map);

    void togglePlayPause();

    bool isPaused() const;

    /*!
     * Requests playback to stop.
     */
    void stop();
    /*!
     * Mutes/Restores volume
     * @param mute state of volume (\b true - mute, \b false - restore)
     */
    void setMuted(bool muted);
    /*!
     * Requests playback to finish.
     */
    void finish();
    /*!
     * Requests a seek to the time \b pos indicated, specified in milliseconds.
     * If \b reset is \b true, this function also clears internal output buffers for faster seeking;
     * otherwise does nothing with buffers.
     */
    void seek(qint64 pos, bool reset = false);

    /*!
     * Returns selected audio parameters.
     */
    AudioParameters audioParameters() const;
    /*!
     * Returns sample size in bytes.
     */
    int sampleSize() const;
    void updateEqSettings();

private:
    void run() override; //thread run function
    void status();
    void dispatch(const PlayState &state);
    void dispatch(const AudioParameters &p);
    void dispatchVisual(Buffer *buffer);
    bool prepareConverters();
//    void startVisualization();
//    void stopVisualization();
    void reset();

private:
    StateHandler        *m_handler              = Q_NULLPTR;
    OutPut::IOutPut     *m_output               = Q_NULLPTR;
    AudioConverter      *m_format_converter     = Q_NULLPTR;
    ChannelConverter    *m_channel_converter    = Q_NULLPTR;
    Recycler            *m_recycler             = Q_NULLPTR;
    EqualizerMgr        *m_eq                   = Q_NULLPTR;
    unsigned char       *m_output_buf           = Q_NULLPTR;

    bool                        m_skip;
    bool                        m_prev_pause;
    bool                        m_useEq;
    std::atomic_bool            m_user_stop;
    std::atomic_bool            m_pause;
    std::atomic_bool            m_finish;
    std::atomic_bool            m_muted;

    int                         m_channels;
    int                         m_kbps;
    quint32                     m_frequency;
    qint64                      m_bytesPerMillisecond;
    qint64                      m_totalWritten;
    qint64                      m_currentMilliseconds;
    size_t                      m_output_size; //samples

    QMutex                      m_mutex;

    ChannelMap                  m_chan_map;

    AudioParameters                 m_in_params;
    AudioParameters::AudioFormat    m_format;
};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

#endif // OUTPUTTHREAD_H
