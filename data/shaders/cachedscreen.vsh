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

uniform vec2 offset;
uniform vec2 subsize;

varying vec2 tex;

void main(void)
{
    gl_Position = vec4(vtxcoord.x + offset.x * 2.0, vtxcoord.y + offset.y * 2.0, 0.0, 1.0);
    tex = vec2((0.5 + 0.5 * vtxcoord.x) * subsize.x, (0.5 + 0.5 * vtxcoord.y) * subsize.y);
}

