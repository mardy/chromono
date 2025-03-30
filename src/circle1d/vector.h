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

#ifndef CARPHONE_VECTOR_H
#define CARPHONE_VECTOR_H

#include "../shadypostproc.h"

#include <math.h>

class Vec2 {
    public:
        explicit Vec2(float x=0.0, float y=0.0)
            : x(x),
              y(y)
        {}

        Vec2 operator+(Vec2 other) {
            return Vec2(x + other.x, y + other.y);
        }

        Vec2 &operator+=(Vec2 other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        Vec2 &operator-=(Vec2 other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        Vec2 operator-(Vec2 other) {
            return Vec2(x - other.x, y - other.y);
        }

        Vec2 operator*(float scalar) {
            return Vec2(x * scalar, y * scalar);
        }

        Vec2 operator*(const Vec2 &other) const {
            return Vec2(x * other.x, y * other.y);
        }

        Vec2 &operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        Vec2 operator/(float scalar) {
            return Vec2(x / scalar, y / scalar);
        }

        Vec2 operator/(const Vec2 &other) const {
            return Vec2(x / other.x, y / other.y);
        }

        Vec2 &operator/=(float scalar) {
            x /= scalar;
            y /= scalar;
            return *this;
        }

        float length() {
            return sqrtf(x*x + y*y);
        }

        bool operator==(Vec2 other) {
            return (*this - other).length() < .0001;
        }

        Vec2 normalize() {
            float l = length();
            if (l == 0) {
                return Vec2(0, 0);
            } else {
                return *this / l;
            }
        }

        Vec2 rotate(float angle) {
            float s = sinf(angle);
            float c = cosf(angle);
            return Vec2(x*c - y*s, x*s + y*c);
        }

        /**
         * Rotate [count] points in [points] (x, y coordinates)
         * around [angle] radians around this vector as center
         **/
        void rotate_around(float angle, float *points, int count) {
            float c = cosf(angle);
            float s = sinf(angle);

            for (int i=0; i<count; i++) {
                float dx = points[(i*2)] - x;
                float dy = points[(i*2) + 1] - y;

                points[(i*2)]   = x + c * dx - s * dy;
                points[(i*2)+1] = y + s * dx + c * dy;
            }
        }

        static Vec2 mean(Vec2 a, Vec2 b) {
            return Vec2((a.x+b.x)/2., (a.y+b.y)/2.);
        }

        float x;
        float y;
};

// http://www.learncpp.com/cpp-tutorial/95-overloading-unary-operators/
inline Vec2 operator-(const Vec2 &other)
{
    return Vec2(-other.x, -other.y);
}

struct Mat4 {
    float m[16];
};

#endif /* CARPHONE_VECTOR_H */
