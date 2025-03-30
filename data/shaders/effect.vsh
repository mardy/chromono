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


attribute vec4 coord;

uniform vec2 subtex;
uniform float brightness;
uniform bool portrait;
uniform float time;

varying vec2 tex;
varying float dim;

void main(void)
{
    dim = brightness;

    gl_Position = vec4(
        2.0 * coord.x - 1.0,
        2.0 * coord.y - 1.0,
        coord.z,
        coord.w
    );

    tex = coord.xy * subtex;
}
