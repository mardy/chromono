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


#include "changewatch.h"

#include "opengl_renderer.h"

#include <string.h>

ChangeWatch::ChangeWatch(void *handle, size_t len)
    : handle(handle)
    , len(len)
    , cached(malloc(len))
{
    memcpy(cached, handle, len);
}

ChangeWatch::ChangeWatch(const ChangeWatch &other)
    : handle(other.handle)
    , len(other.len)
    , cached(malloc(len))
{
    memcpy(cached, handle, len);
}

ChangeWatch::~ChangeWatch()
{
    free(cached);
}

bool
ChangeWatch::needs_update()
{
    if (memcmp(handle, cached, len) != 0) {
        memcpy(cached, handle, len);
        return true;
    }

    return false;
}

