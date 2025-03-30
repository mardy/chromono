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

#ifndef SHADYPOSTPROC_VERTEXACCUMULATOR_H
#define SHADYPOSTPROC_VERTEXACCUMULATOR_H

#include "shadypostproc.h"
#include "chromono_opengl.h"

#include "vertexbuffer.h"

#include <list>

class VertexAccumulator {
    public:
        VertexAccumulator();
        ~VertexAccumulator();

        // Setup
        void add_attrib(GLint location, size_t count);

        // Binding and unbinding helper
        void enable_arrays();
        void disable_arrays();

        void clear();

        // count = number of vertices that are appended
        void append(float *data, size_t count=1);

        // Same as append, but generate triangle strips
        void append_triangle_strip(float *data, size_t count);

        // Returns number of elements to draw
        int upload(VertexBuffer *vertex_buffer);

        size_t stride() { return m_stride; }

    private:
        std::list< std::pair<GLint,size_t> > m_attributes;
        size_t m_buffer_size;
        float *m_buffer;
        size_t m_buffer_offset;
        size_t m_stride;
};

#endif /* SHADYPOSTPROC_VERTEXACCUMULATOR_H */
