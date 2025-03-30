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

#ifndef CIRCLE1D_RGB_H
#define CIRCLE1D_RGB_H

#include "math.h"

#include <algorithm>

struct RGB {
    explicit RGB(int value)
        : r((float)((value >> 16) & 0xff) / 255.)
        , g((float)((value >> 8) & 0xff) / 255.)
        , b((float)((value >> 0) & 0xff) / 255.)
    {
    }

    explicit RGB(float r=0.5, float g=0.5, float b=0.5)
        : r(r), g(g), b(b)
    {}

    static RGB background()
    {
        return RGB(0.25, 0.34, 0.39);
    }

    bool isgray()
    {
        return (r == g && g == b);
    }

    void fade_to(RGB target)
    {
        float alpha = 0.9;
        r = r * alpha + target.r * (1.-alpha);
        g = g * alpha + target.g * (1.-alpha);
        b = b * alpha + target.b * (1.-alpha);
    }

    float diff_to(RGB other)
    {
        float dr = (r - other.r);
        float dg = (g - other.g);
        float db = (b - other.b);
        return sqrtf(dr*dr + dg*dg + db*db);
    }

    bool equals(RGB other)
    {
        return diff_to(other) < 0.01;
    }

    RGB &operator*=(float factor)
    {
        r *= factor;
        g *= factor;
        b *= factor;
        return *this;
    }

    static RGB mix(RGB x, RGB y, float alpha=0.5)
    {
        float beta = 1.0 - alpha;
        float r = x.r * alpha + y.r * beta;
        float g = x.g * alpha + y.g * beta;
        float b = x.b * alpha + y.b * beta;
        return RGB(r, g, b);
    }

    static RGB add(RGB x, RGB y)
    {
        float r = std::min(1.f, x.r + y.r);
        float g = std::min(1.f, x.g + y.g);
        float b = std::min(1.f, x.b + y.b);
        return RGB(r, g, b);
    }

    static RGB desaturate(RGB c)
    {
        float m = (c.r + c.g + c.b) / 3.0;
        return RGB(m, m, m);
    }

    float r;
    float g;
    float b;
};

#endif /* CIRCLE1D_RGB_H */
