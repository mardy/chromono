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


#include "util.h"

#include <stdlib.h>
#include <stdarg.h>

#include "constants.h"

void
Util::format_score_time(long score, char *string, size_t size)
{
    long ms = score * Constants::TICK_MS;
    snprintf(string, size, "%02ld:%02ld.%03ld",
            (ms / 1000) / 60,
            (ms / 1000) % 60,
            ms % 1000);
}

std::string
Util::format(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    std::string result;

    char *tmp;
    if (vasprintf(&tmp, fmt, ap) != -1) {
        result = tmp;
        free(tmp);
    }

    return result;
}
