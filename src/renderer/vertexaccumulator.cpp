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


#include "vertexaccumulator.h"

#define VERTEX_ACCUMULATOR_DEFAULT_SIZE (64*1024)

VertexAccumulator::VertexAccumulator()
    : m_attributes()
    , m_buffer_size(VERTEX_ACCUMULATOR_DEFAULT_SIZE)
    , m_buffer((float*)malloc(m_buffer_size * sizeof(float)))
    , m_buffer_offset(0)
    , m_stride(0)
{
}

VertexAccumulator::~VertexAccumulator()
{
    free(m_buffer);
}

void
VertexAccumulator::add_attrib(GLint location, size_t count)
{
    m_attributes.push_back(std::pair<GLint,size_t>(location, count));
    m_stride += count;
}

void
VertexAccumulator::enable_arrays()
{
    std::list< std::pair<GLint,size_t> >::iterator it;
    int offset = 0;
    for (it=m_attributes.begin(); it != m_attributes.end(); ++it) {
        GLint location = (*it).first;
        size_t count = (*it).second;
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, count, GL_FLOAT, GL_FALSE,
                m_stride * sizeof(float), (void*)(offset * sizeof(float)));
        offset += count;
    }
}

void
VertexAccumulator::disable_arrays()
{
    std::list< std::pair<GLint,size_t> >::iterator it;
    for (it=m_attributes.begin(); it != m_attributes.end(); ++it) {
        GLint location = (*it).first;
        glDisableVertexAttribArray(location);
    }
}

void
VertexAccumulator::clear()
{
    m_buffer_offset = 0;
}

void
VertexAccumulator::append(float *data, size_t count)
{
    count *= m_stride;

    if (m_buffer_size < m_buffer_offset + count) {
        while (m_buffer_size < m_buffer_offset + count) {
            m_buffer_size *= 2;
        }
        m_buffer = (float*)realloc(m_buffer, m_buffer_size * sizeof(float));
    }

    memcpy(m_buffer + m_buffer_offset, data, count * sizeof(float));
    m_buffer_offset += count;
}

void
VertexAccumulator::append_triangle_strip(float *data, size_t count)
{
    for (size_t i=0; i<count - 2; i++) {
        append(data + m_stride * i, 3);
    }
}

int
VertexAccumulator::upload(VertexBuffer *vertex_buffer)
{
    vertex_buffer->data(m_buffer, sizeof(float) * m_buffer_offset);
    return m_buffer_offset / m_stride;
}

