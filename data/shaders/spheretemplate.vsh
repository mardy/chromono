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


attribute vec4 vtxcoord;

uniform vec2 framebuffer_size;
uniform vec2 center;
uniform float size;

varying vec2 pos;

void main(void)
{
    pos  = center + vtxcoord.xy * size;

    // Make position relative to framebuffer without using projection
    gl_Position.x = 2.0 * (pos.x / framebuffer_size.x) - 1.0;
    gl_Position.y = 2.0 * (pos.y / framebuffer_size.y) - 1.0;
    gl_Position.z = 0.0;
    gl_Position.w = 1.0;
}

