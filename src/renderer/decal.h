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

#ifndef SHADYPOSTPROC_DECAL_H
#define SHADYPOSTPROC_DECAL_H

#include "shadypostproc.h"
#include "chromono_opengl.h"

#include "circle1d.h"

#include "renderable.h"
#include "shaderprogram.h"
#include "vertexbuffer.h"
#include "texture.h"

class OpenGLRenderer;

class Decal : public Renderable {
    public:
        Decal(OpenGLRenderer *renderer);
        virtual ~Decal();

        virtual void prepare();

        virtual void bind();
        virtual void unbind();

        void render(enum Circle1D::Decal decal, float x, float y,
                float rotation, float scale, float opacity);

    private:
        Program program;
        VertexBuffer vertex_buffer;

        Texture texture;

        GLint vtxcoord_loc;
        GLint texcoord_loc;
        GLint opacity_loc;
};

#endif /* SHADYPOSTPROC_DECAL_H */
