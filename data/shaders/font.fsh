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


varying vec2 tex;
varying vec4 col;

uniform vec4 transform;

uniform sampler2D sampler;

void main(void)
{
    // The bigger the text scale, the smaller we have to make the alpha border
    // Scale is stored in "z" of transform
    float e = 0.05 / transform.z;

    float dist = texture2D(sampler, tex).a;
    float alpha = smoothstep(0.5 - e, 0.5 + e, dist);
    gl_FragColor = vec4(col.rgb, col.a * alpha);
}

