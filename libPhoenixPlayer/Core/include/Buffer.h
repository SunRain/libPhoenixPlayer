// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef BUFFER_H
#define BUFFER_H

#include <QtGlobal>

namespace PhoenixPlayer {


/*! @brief Audio buffer class.
 * @author Brad Hughes <bhughes@trolltech.com>
 */
class Buffer
{
public:

    const static uint BUFFER_PERIOD = 512;

    /*!
     * Constructs an empty buffer object.
     * @param sz Size in bytes;
     */
    Buffer(size_t sz)
    {
        data = new float[sz];
        samples = 0;
        rate = 0;
        size = sz;
    }
    /*!
     * Destructor.
     */
    ~Buffer()
    {
        delete[] data;
        data = nullptr;
        samples = 0;
        rate = 0;
        size = 0;
    }

    float *data;      /*!< Audio data */
//    ulong nbytes;     /*!< Audio data size */
    size_t samples;                      /*!< Audio data size in samples. */
//    unsigned long rate;       /*!< Buffer bitrate */
    size_t size;       /*!< Buffer size */
    unsigned int rate;                   /*!< Buffer bitrate. */
};
} //PhoenixPlayer
#endif // BUFFER_H
