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


#include "projection.h"

#include <algorithm>

Projection::Projection(int width, int height)
    : size(width, height)
    , world_size(Constants::WORLD_WIDTH, Constants::WORLD_HEIGHT)
    , world_offset(0.0, 0.0)
    , projection()
{
    float screen_aspect = size.x / size.y;
    float world_aspect = world_size.x / world_size.y;

    if (world_aspect > screen_aspect) {
        // world broader than screen - top and bottom letterbox
        float screen_height_in_world_coordinates = size.y * world_size.x / size.x;
        world_offset.y += (screen_height_in_world_coordinates - world_size.y) / 2.0;
    } else if (world_aspect < screen_aspect) {
        // world taller than screen - left and right letterbox
        float screen_width_in_world_coordinates = size.x * world_size.y / size.y;
        world_offset.x += (screen_width_in_world_coordinates - world_size.x) / 2.0;
    }

    float left = 0.0 - world_offset.x;
    float right = world_size.x + world_offset.x;
    float top = 0.0 - world_offset.y;
    float bottom = world_size.y + world_offset.y;
    float near = -1.f;
    float far = +1.f;

    memset(projection.m, 0, sizeof(projection.m));
    projection.m[0] = 2.f / (right - left);
    projection.m[3] = - (right + left) / (right - left);
    projection.m[5] = 2.f / (top - bottom);
    projection.m[7] = - (top + bottom) / (top - bottom);
    projection.m[10] = -2.f / (far - near);
    projection.m[11] = - (far + near) / (far - near);
    projection.m[15] = 1.f;
}

Mat4
Projection::matrix(bool with_rotation)
{
    return projection;
}

Vec2
Projection::screen2world(Vec2 screen)
{
    return ((screen / size) * (world_size + world_offset * 2.f)) - world_offset;
}

