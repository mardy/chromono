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


#include "vertexbuffer.h"

VertexBuffer *
VertexBuffer::active = NULL;

VertexBuffer::VertexBuffer()
    : m_id(0)
{
    glGenBuffers(1, &m_id);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_id);
}

void
VertexBuffer::bind()
{
    if (active != this) {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        active = this;
    }
}

void
VertexBuffer::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    active = NULL;
}

void
VertexBuffer::data(float *data, size_t size, bool is_static)
{
    VertexBuffer *old_active = active;

    bind();
    glBufferData(GL_ARRAY_BUFFER, size, data,
            is_static?GL_STATIC_DRAW:GL_STREAM_DRAW);

    if (old_active == NULL) {
        unbind();
    } else if (old_active != this) {
        old_active->bind();
    }
}

