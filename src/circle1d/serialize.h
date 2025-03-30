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

#ifndef CIRCLE1D_SERIALIZE_H
#define CIRCLE1D_SERIALIZE_H

#include <cstdint>
#include <stdlib.h>

#include "rgb.h"
#include "vector.h"

#include <string>

class SerializeBufferPriv;

class SerializeBuffer {
public:
    SerializeBuffer();
    ~SerializeBuffer();

    void clear();
    void rewind();
    const char *data(size_t *len);

    SerializeBuffer &operator<<(float value) {
        write_float32(value);
        return *this;
    }

    SerializeBuffer &operator<<(int32_t value) {
        write_int32(value);
        return *this;
    }

    SerializeBuffer &operator<<(int64_t value) {
        write_int64(value);
        return *this;
    }

    SerializeBuffer &operator<<(const std::string &value) {
        write_string(value);
        return *this;
    }

    SerializeBuffer &operator<<(RGB value) {
        write_rgb(value);
        return *this;
    }

    SerializeBuffer &operator<<(Vec2 value) {
        write_vec2(value);
        return *this;
    }

    void write_float32(float value);
    void write_int32(int32_t value);
    void write_int64(int64_t value);
    void write_string(const std::string &value);
    void write_rgb(RGB value) {
        write_float32(value.r);
        write_float32(value.g);
        write_float32(value.b);
    }
    void write_vec2(Vec2 value) {
        write_float32(value.x);
        write_float32(value.y);
    }

    SerializeBuffer &operator>>(float &value) {
        value = read_float32();
        return *this;
    }

    SerializeBuffer &operator>>(int32_t &value) {
        value = read_int32();
        return *this;
    }

    SerializeBuffer &operator>>(int64_t &value) {
        value = read_int64();
        return *this;
    }

    SerializeBuffer &operator>>(std::string &value) {
        value = read_string();
        return *this;
    }

    SerializeBuffer &operator>>(RGB &value) {
        value = read_rgb();
        return *this;
    }

    SerializeBuffer &operator>>(Vec2 &value) {
        value = read_vec2();
        return *this;
    }

    size_t offset();
    size_t available();

    float read_float32();
    int32_t read_int32();
    int64_t read_int64();
    std::string read_string();
    RGB read_rgb() {
        float r = read_float32();
        float g = read_float32();
        float b = read_float32();
        return RGB(r, g, b);
    }
    Vec2 read_vec2() {
        float x = read_float32();
        float y = read_float32();
        return Vec2(x, y);
    }

    void write_file(const std::string &filename);
    void read_file(const std::string &filename);

private:
    SerializeBufferPriv *priv;
};

#endif /* CIRCLE1D_SERIALIZE_H */
