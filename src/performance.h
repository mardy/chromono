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

#ifndef SHADYPOSTPROC_PERFORMANCE_H
#define SHADYPOSTPROC_PERFORMANCE_H

#include "shadypostproc.h"

class Performance {
    public:
        Performance();
        void frame();

        float framerate() { return m_frame_rate; }
        void set_show_fps(bool show_fps) { m_show_fps = show_fps; }

    private:
        long m_last_reset;
        long m_frames;
        float m_frame_rate;
        bool m_show_fps;
};

#endif /* SHADYPOSTPROC_PERFORMANCE_H */
