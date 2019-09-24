/***************************************************************************
 *   Copyright (C) 2014 by Ilya Kotov                                      *
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

#include <QStringList>
#include <QHash>
#include "ChannelMap.h"


namespace PhoenixPlayer {

    namespace PlayBackend {

        namespace PhoenixBackend {

ChannelPosition ChannelMap::m_internal_map[9] = { CHAN_FRONT_LEFT,
                                                        CHAN_FRONT_RIGHT,
                                                        CHAN_REAR_LEFT,
                                                        CHAN_REAR_RIGHT,
                                                        CHAN_FRONT_CENTER,
                                                        CHAN_REAR_CENTER,
                                                        CHAN_LFE,
                                                        CHAN_SIDE_LEFT,
                                                        CHAN_SIDE_RIGHT };
ChannelMap::ChannelMap() {}

ChannelMap::ChannelMap(int channels)
{
    generateMap(channels);
}

int ChannelMap::mask() const
{
    int mask = 0;
    foreach (ChannelPosition channel, *this)
    {
        mask |= channel;
    }
    return mask;
}

const ChannelMap ChannelMap::remaped() const
{
    ChannelMap map;
    for(int i = 0; i < 9; ++i)
    {
         if(contains(m_internal_map[i]))
             map.append(m_internal_map[i]);
    }
    while (map.count() < count())
    {
        map.append(CHAN_NULL);
    }
    return map;
}

const QString ChannelMap::toString() const
{
    QStringList list;
    QHash <ChannelPosition, QString> names;
    names.insert(CHAN_NULL, "NA");
    names.insert(CHAN_FRONT_LEFT, "FL");
    names.insert(CHAN_FRONT_RIGHT, "FR");
    names.insert(CHAN_REAR_LEFT, "RL");
    names.insert(CHAN_REAR_RIGHT, "RR");
    names.insert(CHAN_FRONT_CENTER, "FC");
    names.insert(CHAN_REAR_CENTER, "RC");
    names.insert(CHAN_LFE, "LFE");
    names.insert(CHAN_SIDE_LEFT, "SL");
    names.insert(CHAN_SIDE_RIGHT, "SR");


    foreach (ChannelPosition channel, *this)
    {
       list << names.value(channel);
    }
    return list.join(",");
}

void ChannelMap::generateMap(int channels)
{
    int mask = 0;

    switch (channels)
    {
    case 1:
        mask = CHAN_FRONT_LEFT;
        break;
    case 2:
        mask = CHAN_FRONT_LEFT
                | CHAN_FRONT_RIGHT;
        break;
    case 3:
        mask = CHAN_FRONT_LEFT
                | CHAN_FRONT_RIGHT
                | CHAN_FRONT_CENTER;
        break;
    case 4:
        mask = CHAN_FRONT_LEFT
                | CHAN_FRONT_RIGHT
                | CHAN_REAR_LEFT
                | CHAN_REAR_RIGHT;
        break;
    case 5:
        mask = CHAN_FRONT_LEFT
                | CHAN_FRONT_RIGHT
                | CHAN_FRONT_CENTER
                | CHAN_REAR_LEFT
                | CHAN_REAR_RIGHT;
        break;
    case 6:
        mask = CHAN_FRONT_LEFT
                | CHAN_FRONT_RIGHT
                | CHAN_FRONT_CENTER
                | CHAN_LFE
                | CHAN_REAR_LEFT
                | CHAN_REAR_RIGHT;
        break;
    case 7:
        mask = CHAN_FRONT_LEFT
                | CHAN_FRONT_RIGHT
                | CHAN_FRONT_CENTER
                | CHAN_LFE
                | CHAN_REAR_CENTER
                | CHAN_SIDE_LEFT
                | CHAN_SIDE_RIGHT;
        break;
    case 8:
        mask = CHAN_FRONT_LEFT
                | CHAN_FRONT_RIGHT
                | CHAN_FRONT_CENTER
                | CHAN_LFE
                | CHAN_REAR_LEFT
                | CHAN_REAR_RIGHT
                | CHAN_SIDE_LEFT
                | CHAN_SIDE_RIGHT;
        break;
    default:
        ;
    }

    for(int i = 0; i < 9; ++i)
    {
        if(mask & m_internal_map[i])
            append(m_internal_map[i]);
    }
}

} //PhoenixPlayer
} //PlayBackend
} //PhoenixPlayer
