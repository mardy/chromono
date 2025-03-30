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

#ifndef SHADYPOSTPROC_UTIL_H
#define SHADYPOSTPROC_UTIL_H

#include "shadypostproc.h"

#include <string>

class Util {
    public:
        /* Number of milliseconds since program start */
        static long ticks();

        static void format_score_time(long score,
                char *string, size_t size);

        /* allocating sprintf */
        static std::string format(const char *fmt, ...);
};

class Random {
    /* http://en.wikipedia.org/wiki/Random_number_generation */
    public:
        Random(int w, int z) : w(w), z(z) { }

        unsigned int next() {
            z = 36969 * (z & 65535) + (z >> 16);
            w = 18000 * (w & 65535) + (w >> 16);
            return (z << 16) + w; /* 32-bit result */
        }

    private:
        int w;
        int z;
};

#endif /* SHADYPOSTPROC_UTIL_H */
