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

#ifndef SHADYPOSTPROC_VERTEXBUFFER_H
#define SHADYPOSTPROC_VERTEXBUFFER_H

#include "shadypostproc.h"
#include "chromono_opengl.h"

class VertexBuffer {
    public:
        VertexBuffer();
        virtual ~VertexBuffer();

        void bind();
        void unbind();

        void data(float *data, size_t size, bool is_static=false);

    private:
        GLuint m_id;

        static VertexBuffer *active;
};

#endif /* SHADYPOSTPROC_VERTEXBUFFER_H */
