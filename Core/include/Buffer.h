// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//

#ifndef BUFFER_H
#define BUFFER_H

#include <QtGlobal>

namespace PhoenixPlayer {

#define BUFFER_PERIOD 512

/*! @brief Audio buffer class.
 * @author Brad Hughes <bhughes@trolltech.com>
 */
class Buffer
{
public:
    /*!
     * Constructs an empty buffer object.
     * @param sz Size in bytes;
     */
    Buffer(unsigned long sz)
    {
        data = new unsigned char[sz];
        nbytes = 0;
        rate = 0;
        size = sz;
    }
    /*!
     * Destructor.
     */
    ~Buffer()
    {
        delete[] data;
        data = 0;
        nbytes = 0;
        rate = 0;
        size = 0;
    }

    unsigned char *data;      /*!< Audio data */
    unsigned long nbytes;     /*!< Audio data size */
    unsigned long rate;       /*!< Buffer bitrate */
    unsigned long size;       /*!< Buffer size */
};
} //PhoenixPlayer
#endif // BUFFER_H
