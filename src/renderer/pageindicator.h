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

#ifndef SHADYPOSTPROC_PAGEINDICATOR_H
#define SHADYPOSTPROC_PAGEINDICATOR_H

#include "shadypostproc.h"
#include "chromono_opengl.h"

#include "renderable.h"
#include "shaderprogram.h"
#include "vertexbuffer.h"

class OpenGLRenderer;

class PageIndicator : public Renderable {
    public:
        PageIndicator(OpenGLRenderer *renderer);
        virtual ~PageIndicator();

        virtual void bind();
        void render(int x, int y, int page, int pages);
        virtual void unbind();

    private:
        Program program;
        VertexBuffer vertex_buffer;
        int size;
        int spacing;

        GLint vtxcoord_loc;
        GLint center_loc;
        GLint size_loc;
        GLint filled_loc;
};

#endif /* SHADYPOSTPROC_PAGEINDICATOR_H */
