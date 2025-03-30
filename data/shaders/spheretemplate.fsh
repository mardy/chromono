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


varying vec2 pos;

uniform vec2 center;
uniform float size;

// red = desired color intensity
// green = color intensity
// blue = highlight

void main(void)
{
    // Distance of this point to the center
    float dist = length(pos - center);

    // Shadow effect
    float shadow_border = 0.005 * size;
    float shadow_dist = length(pos - (center + vec2(shadow_border)));

    // Distance of this point to the highlight center
    float highlight = length(pos - (center - 0.2 * size));

    // Border width for antialiased sphere border
    float border = 2.7;

    // Alpha value determines the shape of the sphere
    gl_FragColor.rgba = vec4(smoothstep(size, size-border, dist));
    if (shadow_dist < dist) {
        gl_FragColor.a += smoothstep(size-shadow_border, size-border, shadow_dist);
    }

    // Intensity of highlight
    float intensity = 0.15;
    highlight = smoothstep(size, size/2.0, highlight);

    // Color is based on original color + highlight effect
    //gl_FragColor.rgb = vec3(intensity) * highlight;
    gl_FragColor.b *= intensity * highlight;

    float mixer = smoothstep(size/2.0, size/2.0 - border, dist);
    gl_FragColor.r *= mixer;
    gl_FragColor.g *= (1.0 - mixer);
}

