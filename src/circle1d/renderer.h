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

#ifndef CIRCLE1D_RENDERER_H
#define CIRCLE1D_RENDERER_H

#include "rgb.h"

class Object;
class Joint;

enum FontSize {
    FONT_SMALL,
    FONT_MEDIUM,
    FONT_LARGE,
    FONT_XLARGE,
    FONT_TINY,
};

class Circle1DRenderer {
    public:
        Circle1DRenderer() {}
        virtual ~Circle1DRenderer() {}

        virtual void begin() = 0;

        virtual void circle(Object *o) = 0;
        virtual void flush_circles() = 0;

        virtual void line(Joint *j) = 0;
        virtual void flush_lines() = 0;

        virtual void text_measure(const char *text, float *width, float *height, enum FontSize size) = 0;
        virtual void text_render(const char *text, float x, float y, enum FontSize size, float opacity=1.0, RGB color=RGB(1.0, 1.0, 1.0)) = 0;
        virtual void decal(int decal, int x, int y, float opacity) = 0;

        virtual void shadow(Vec2 light, Vec2 sphere, float size) = 0;
        virtual void flush_shadows() = 0;

        virtual void finish() = 0;
};

#endif /* CIRCLE1D_RENDERER_H */
