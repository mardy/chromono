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

#include "serialize.h"

#include <string.h>
#include <stdlib.h>

#include <fstream>

class SerializeBufferPriv {
public:
    SerializeBufferPriv();
    ~SerializeBufferPriv();

    void write(const void *data, size_t size);
    void *read(size_t size);

    void clear();
    void rewind();

    size_t length();
    void *buffer();
    size_t offset();

private:
    size_t m_buffer_size;
    char *m_buffer;
    size_t m_pos;
    size_t m_length;
};

SerializeBufferPriv::SerializeBufferPriv()
    : m_buffer_size(1024 * 64)
    , m_buffer((char *)malloc(m_buffer_size))
    , m_pos(0)
    , m_length(0)
{
}

SerializeBufferPriv::~SerializeBufferPriv()
{
    free(m_buffer);
}

void
SerializeBufferPriv::write(const void *data, size_t size)
{
    m_length += size;

    if (m_buffer_size < m_length) {
        while (m_buffer_size < m_length) {
            m_buffer_size *= 2;
        }

        m_buffer = (char *)realloc(m_buffer, m_buffer_size);
    }

    memcpy(m_buffer + m_pos, data, size);
    m_pos += size;
}

void *
SerializeBufferPriv::read(size_t size)
{
    SHADY_ASSERT(m_pos + size <= m_length);
    void *result = m_buffer + m_pos;
    m_pos += size;
    return result;
}

void
SerializeBufferPriv::clear()
{
    m_pos = m_length = 0;
}

void
SerializeBufferPriv::rewind()
{
    m_pos = 0;
}

size_t
SerializeBufferPriv::length()
{
    return m_length;
}

void *
SerializeBufferPriv::buffer()
{
    return m_buffer;
}

size_t
SerializeBufferPriv::offset()
{
    return m_pos;
}


SerializeBuffer::SerializeBuffer()
    : priv(new SerializeBufferPriv())
{
}

SerializeBuffer::~SerializeBuffer()
{
    delete priv;
}

void
SerializeBuffer::clear()
{
    priv->clear();
}

void
SerializeBuffer::rewind()
{
    priv->rewind();
}

const char *
SerializeBuffer::data(size_t *len)
{
    *len = priv->length();
    return (const char *)priv->buffer();
}

void
SerializeBuffer::write_float32(float value)
{
    // TODO endianness conversion
    priv->write(&value, sizeof(float));
}

void
SerializeBuffer::write_int32(int32_t value)
{
    // TODO endianness conversion
    priv->write(&value, sizeof(int32_t));
}

void
SerializeBuffer::write_int64(int64_t value)
{
    // TODO endianness conversion
    priv->write(&value, sizeof(int64_t));
}

void
SerializeBuffer::write_string(const std::string &value)
{
    write_int32(value.length() + 1);
    priv->write(value.c_str(), value.length() + 1);
}

float
SerializeBuffer::read_float32()
{
    float result = *((float *)priv->read(sizeof(float)));
    // TODO endianness conversion
    return result;
}

int32_t
SerializeBuffer::read_int32()
{
    int32_t result = *((int32_t *)priv->read(sizeof(int32_t)));
    // TODO endianness conversion
    return result;
}

int64_t
SerializeBuffer::read_int64()
{
    int64_t result = *((int64_t *)priv->read(sizeof(int64_t)));
    // TODO endianness conversion
    return result;
}

std::string
SerializeBuffer::read_string()
{
    int32_t length = read_int32();
    char *buf = (char *)malloc(length);
    memcpy(buf, priv->read(length), length);
    std::string result(buf);
    free(buf);
    return result;
}

size_t
SerializeBuffer::offset()
{
    return priv->offset();
}

void
SerializeBuffer::write_file(const std::string &filename)
{
    std::fstream out;

    out.open(filename.c_str(), std::fstream::out | std::fstream::binary);
    out.write((const char *)priv->buffer(), priv->length());
    out.close();
}

void
SerializeBuffer::read_file(const std::string &filename)
{
    std::fstream in;

    in.open(filename.c_str(), std::fstream::in | std::fstream::binary);

    if (!in.is_open()) {
        return;
    }

    // Determine file size
    in.seekg(0, std::fstream::end);
    size_t len = in.tellg();

    // Read data from file
    char *tmp = (char *)malloc(len);
    in.seekg(0, std::fstream::beg);
    in.read(tmp, len);
    in.close();

    // Write data to priv
    priv->clear();
    priv->write(tmp, len);
    priv->rewind();
    free(tmp);
}

size_t
SerializeBuffer::available()
{
    return priv->length() - priv->offset();
}
