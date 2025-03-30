/**
 * chro.mono: A very circular color puzzle game
 * https://thp.io/2013/chromono/
 * Copyright (C) 2013-2021 Thomas Perl <m@thp.io>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **/


#include "performance.h"

#include "util.h"


Performance::Performance()
    : m_last_reset(Util::ticks())
    , m_frames(0)
    , m_frame_rate(0.0)
    , m_show_fps(false)
{
}

void
Performance::frame()
{
    m_frames++;

    long now = Util::ticks();
    long diff = (now - m_last_reset);
    if (diff > 1000) {
        m_frame_rate = (float)m_frames * 1000.f / (float)diff;
        if (m_show_fps) {
            SHADY_DEBUG_PRINTF("FPS: %.2f\n", m_frame_rate);
        }
        m_last_reset = now;
        m_frames = 0;
    }
}

